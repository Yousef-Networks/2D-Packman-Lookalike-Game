// Includes
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <random>
#include <glut.h>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
#include <string>





int windowWidth = 1200;
int windowHeight = 800;
int playerPositionx = windowWidth / 2;
int playerPositiony = windowHeight / 2;
float playerRotationAngle = 0.0f; 
float lives = 5.0;
int score = 0;
int currentTime = 60000;
float cloudX = 0.0f;   // Initial X position of the clouds
const float cloudSpeed = 0.01f; // Speed at which clouds move

float collectableRotation = 0.0f;
bool gameOver = false;
bool win = false;
float goalrotationAngle = 0.0f;
float goalrotationSpeed = 3.0f; 
bool goalrotateRight = true;
bool shieldOn = false;
float powerUpVerticalOffset;

struct Point {
	float x;
	float y;
};

struct Star {
	float x;
	float y;
	float speed;
};


std::vector<Point> obstacles;
std::vector<Point> collectables;
std::vector<Point> powerups;
std::vector<Point> goal;
Point goalpoint;
Point player;




// draws rectangles using the (x,y) of the bottom left vertex, width (w) and height (h)
void drawRectangle(int x, int y, int w, int h) {
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();
}

// draws a circle using OpenGL's gluDisk, given (x,y) of its center and tis radius
void drawCircle(int x, int y, float r) {
	glPushMatrix();
	glTranslatef(x, y, 0);
	GLUquadric* quadObj = gluNewQuadric();
	gluDisk(quadObj, 0, r, 50, 50);
	glPopMatrix();
}

void drawText(float x, float y, std::string text) {
	glRasterPos2f(x, y);

	for (char c : text) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
}


void drawPlayer() {
	glPushMatrix();
	glTranslatef(player.x, player.y, 0); // Translate to the player's position
	glRotatef(playerRotationAngle, 0, 0, 1);

	// Draw the player's body (rectangle) in grey
	glColor3f(0.5, 0.5, 0.5); // Grey color for the rectangle
	drawRectangle(-50, -40, 100, 80); // Adjust the size as needed

	// Draw the player's face (circle) in blue
	glColor3f(0.0, 0.0, 1.0); // Blue color for the face
	drawCircle(0, -10, 27.0f); // Adjust the radius (27.0f) and position as needed

	// Draw two eyes
	glColor3f(0.0, 0.0, 0.0); // Black color for the eyes
	glPointSize(5.0); // Adjust point size as needed
	glBegin(GL_POINTS);
	glVertex2f(-10.0f, 20.0f); // Left eye
	glVertex2f(10.0f, 20.0f); // Right eye
	glEnd();

	// Draw a slightly bent down mouth (a curved line)
	glColor3f(0.0, 0.0, 0.0); // Black color for the mouth
	glLineWidth(2.0); // Adjust line width as needed
	glBegin(GL_LINE_STRIP);
	for (int i = -15; i <= 15; i++) {
		glVertex2f(i, -15 - 0.05 * i * i); // Adjust the curve parameters as needed
	}
	glEnd();

	glPopMatrix();
}



void drawGreenBox(float x, float y, float halfWidth, float halfHeight) {
	// Calculate the vertices of the green box
	float left = x - halfWidth;
	float right = x + halfWidth;
	float top = y + halfHeight;
	float bottom = y - halfHeight;

	// Draw a green box
	glColor3f(0.0, 1.0, 0.0); // Green color
	glBegin(GL_QUADS);
	glVertex2f(left, top);
	glVertex2f(right, top);
	glVertex2f(right, bottom);
	glVertex2f(left, bottom);
	glEnd();

	// Draw a black "X" inside the green box
	glLineWidth(3.0); // Adjust line width as needed
	glColor3f(0.0, 0.0, 0.0); // Black color
	glBegin(GL_LINES);

	// Top-left to bottom-right diagonal of "X"
	glVertex2f(left, top);
	glVertex2f(right, bottom);

	// Top-right to bottom-left diagonal of "X"
	glVertex2f(right, top);
	glVertex2f(left, bottom);

	glEnd();
}


void drawObstacles() {
	float boxWidth = 50.0;
	float boxHeight = 50.0;

	for (int i = 0; i < obstacles.size(); i++) {
		drawGreenBox(obstacles[i].x, obstacles[i].y, boxWidth / 2, boxHeight / 2);
	}

}

void drawBlackLine(float x1, float y1, float x2, float y2) {
	glColor3f(0.0, 1.0, 0.0); // Black color for the line
	glLineWidth(7.0); // Adjust line width as needed
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}

void drawCircle(float x, float y, float radius, int segments) {
	glColor3f(0.0, 1.0, 0.0); // Black color for the circle
	glLineWidth(7.0);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < segments; i++) {
		float theta = 2.0f * 3.14 * float(i) / float(segments);
		float x1 = radius * cosf(theta);
		float y1 = radius * sinf(theta);
		glVertex2f(x + x1, y + y1);
	}
	glEnd();
}




void drawStar(float x, float y, float size) {
	
	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(collectableRotation, 0, 0, 1); // Rotate the collectable
	glTranslatef(-x, -y, 0);
	glColor3f(1.0, 0.0, 1.0); // purple color

	// Define angles for the five points of the star
	float angles[] = { 72.0, 144.0, 216.0, 288.0, 0.0 };

	// Draw the star
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 5; i++) {
		float angle = angles[i] * 3.14 / 180.0; // Convert to radians
		float x1 = x + size * sin(angle);
		float y1 = y + size * cos(angle);

		float nextAngle = angles[(i + 1) % 5] * 3.14 / 180.0; // Convert to radians
		float x2 = x + size * sin(nextAngle);
		float y2 = y + size * cos(nextAngle);

		glVertex2f(x, y);
		glVertex2f(x1, y1);
		glVertex2f(x2, y2);
	}


	glEnd();
	glPopMatrix();
}
void drawBoundaries() {
	// Set the color to red (R, G, B values)
	glColor3f(1.0, 0.0, 0.0); // Red color

	drawBlackLine(0, windowHeight, windowWidth, windowHeight);
	drawCircle(0, 0, 20, 50);

	// Top-right corner
	drawBlackLine(windowWidth, windowHeight, windowWidth, 0);
	drawCircle(windowWidth, 0, 20, 50);

	// Bottom-left corner
	drawBlackLine(0, 0, windowWidth, 0);
	drawCircle(0, windowHeight, 20, 50);

	// Bottom-right corner
	drawBlackLine(0, 0, 0, windowHeight);
	drawCircle(windowWidth, windowHeight, 20, 50);
}

//  a shield power-up



void drawCollectables() {
	for (int i = 0; i < collectables.size(); i++) {
		drawStar(collectables[i].x, collectables[i].y, 20.0);
	}
}


void drawGoal(float x, float y, float width, float height, float angle) {
	// Draw the rocket body (gray rectangle)
	glColor3f(0.7, 0.7, 0.7); // Gray color
	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(angle, 0, 0, 1); // Rotate the rocket
	glTranslatef(-x, -y, 0);
	glBegin(GL_POLYGON);
	glVertex2f(x - width / 2, y - height / 2);
	glVertex2f(x + width / 2, y - height / 2);
	glVertex2f(x + width / 2, y + height / 2);
	glVertex2f(x - width / 2, y + height / 2);
	glEnd();
	glPopMatrix();

	// Draw the rocket tip (red triangle)
	glColor3f(0.9, 0.0, 0.0); // Red color
	glBegin(GL_POLYGON);
	glVertex2f(x - width / 2, y + height / 2);
	glVertex2f(x + width / 2, y + height / 2);
	glVertex2f(x, y + height);
	glEnd();

	// Draw the rocket window (blue circle)
	glColor3f(0.0, 0.0, 1.0); // Blue color
	const float radius = 15.0;
	const float centerX = x;
	const float centerY = y + height / 4;
	glPushMatrix();
	glTranslatef(centerX, centerY, 0);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++) {
		float angle = i * 3.14 / 180.0; // Convert to radians
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		glVertex2f(x, y);
	}
	glEnd();
	glPopMatrix();

	// Draw the rocket fins (gray lines)
	glColor3f(0.5, 0.5, 0.5); // Gray color
	glLineWidth(3.0); // Set line width
	glBegin(GL_LINES);
	glVertex2f(x - width / 2, y - height / 2);
	glVertex2f(x - width / 2 - 25, y - height / 2 + 25);
	glVertex2f(x - width / 2, y - height / 2);
	glVertex2f(x - width / 2 + 25, y - height / 2 + 25);
	glVertex2f(x + width / 2, y - height / 2);
	glVertex2f(x + width / 2 - 25, y - height / 2 + 25);
	glVertex2f(x + width / 2, y - height / 2);
	glVertex2f(x + width / 2 + 25, y - height / 2 + 25);
	glEnd();

	// Draw the rocket flames (orange squares)
	glColor3f(1.0, 0.6, 0.0); // Orange color
	glBegin(GL_POLYGON);
	glVertex2f(x - 10, y - height / 2);
	glVertex2f(x - 5, y - height / 2);
	glVertex2f(x - 5, y - height / 2 - 20);
	glVertex2f(x - 10, y - height / 2 - 20);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(x, y - height / 2);
	glVertex2f(x + 5, y - height / 2);
	glVertex2f(x + 5, y - height / 2 - 20);
	glVertex2f(x, y - height / 2 - 20);
	glEnd();
}

void Shield(float x, float y, float size) {


	y += powerUpVerticalOffset;
	glColor3f(255.0 / 255.0, 215.0 / 255.0, 0.0 / 255.0);
	// Golden

	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++) {
		float angle = i * 3.14159265 / 180.0; // Convert to radians
		float x1 = x + size * cos(angle);
		float y1 = y + size * sin(angle);
		glVertex2f(x1, y1);
	}
	glEnd();




	glColor3f(139.0 / 255.0, 69.0 / 255.0, 19.0 / 255.0); // Brown

	// Draw the diamond inside the shield
	glBegin(GL_POLYGON);
	glVertex2f(x, y - size / 2); // Top point
	glVertex2f(x - size / 2, y); // Left point
	glVertex2f(x, y + size / 2); // Bottom point
	glVertex2f(x + size / 2, y); // Right point
	glEnd();
}




void HealthBar() {
	// Calculate the width of the health bar based on the player's health
	float barWidth = (lives / 5) * (windowWidth / 4);

	// Draw the health bar background (red)
	glColor3f(1.0, 0.0, 0.0); // Red color
	glBegin(GL_POLYGON);
	glVertex2f(15, windowHeight - 50);
	glVertex2f(15 + windowWidth / 4, windowHeight - 50);
	glVertex2f(15 + windowWidth / 4, windowHeight - 30);
	glVertex2f(15, windowHeight - 30);
	glEnd();

	// Draw the health bar (green) on top of the background
	glColor3f(0.0, 1.0, 0.0); // Green color
	glBegin(GL_POLYGON);
	glVertex2f(15, windowHeight - 50);
	glVertex2f(15 + barWidth, windowHeight - 50);
	glVertex2f(15 + barWidth, windowHeight - 30);
	glVertex2f(15, windowHeight - 30);
	glEnd();
}


// Draw an hourglass power-up
void drawHourglass(float x, float y, float size) {
	y += powerUpVerticalOffset;
	float halfSize = size / 2;

	// Draw the top triangle (brown)
	glColor3f(0.647, 0.164, 0.164); // Brown color
	glBegin(GL_TRIANGLES);
	glVertex2f(x, y + halfSize);
	glVertex2f(x - halfSize, y - halfSize);
	glVertex2f(x + halfSize, y - halfSize);
	glEnd();

	// Draw the bottom triangle (brown)
	glColor3f(0.647, 0.164, 0.164); // Brown color
	glBegin(GL_TRIANGLES);
	glVertex2f(x, y - halfSize);
	glVertex2f(x - halfSize, y + halfSize);
	glVertex2f(x + halfSize, y + halfSize);
	glEnd();

	// Draw the small rectangle in the middle (brown)
	glColor3f(1.0, 0.85, 0.2); // Brown color
	glBegin(GL_POLYGON);
	glVertex2f(x - size / 4, y - size / 4);
	glVertex2f(x + size / 4, y - size / 4);
	glVertex2f(x + size / 4, y + size / 4);
	glVertex2f(x - size / 4, y + size / 4);
	glEnd();
}

void drawPowerUps() {
	for (size_t i = 0; i < powerups.size(); i++) {
		if (i % 2 == 0) {
			Shield(powerups[i].x, powerups[i].y, 20.0); // Adjust size as needed
		}
		else {
			drawHourglass(powerups[i].x, powerups[i].y, 25.0); // Adjust size as needed
		}
	}
}


bool isOverlapping(float x, float y, std::vector<Point>& objects) {
	// Check if the new position (x, y) overlaps with any existing object
	for (const Point& obj : objects) {
		if (abs(x - obj.x) < 30 && abs(y - obj.y) < 30) {
			return true; // Overlapping
		}
	}
	return false; // Not overlapping
}


void initializeObstacles() {
	for (int i = 0; i < 5; i++) {
		float x, y;
		bool overlapping;
		do {
			x = rand() % (windowWidth - 50);
			y = rand() % (windowHeight - 50);
			overlapping = isOverlapping(x, y, obstacles) || isOverlapping(x, y, collectables) || isOverlapping(x, y, powerups) || isOverlapping(x, y, goal);
		} while (overlapping);

		Point obstacle;
		obstacle.x = x;
		obstacle.y = y;
		obstacles.push_back(obstacle);
	}
}

void initializeCollectables() {
	for (int i = 0; i < 6; i++) {
		float x, y;
		bool overlapping;
		do {
			x = rand() % (windowWidth - 100);
			y = rand() % (windowHeight - 150);
			overlapping = isOverlapping(x, y, collectables) || isOverlapping(x, y, obstacles) || isOverlapping(x, y, powerups) || isOverlapping(x, y, goal);
		} while (overlapping);

		Point collectable;
		collectable.x = x;
		collectable.y = y;
		collectables.push_back(collectable);
	}
}

void initializeGoal() {
	float x, y;
	bool overlapping;
	do {
		x = rand() % (windowWidth - 100);
		y = rand() % (windowHeight - 150);
		overlapping = isOverlapping(x, y, goal) || isOverlapping(x, y, obstacles) || isOverlapping(x, y, collectables) || isOverlapping(x, y, powerups);
	} while (overlapping);

	goalpoint.x = x;
	goalpoint.y = y;
}

void initializePowerups() {
	for (int i = 0; i < 4; i++) {
		float x, y;
		bool overlapping;
		do {
			x = rand() % (windowWidth - 80);
			y = rand() % (windowHeight - 120);
			overlapping = isOverlapping(x, y, powerups) || isOverlapping(x, y, obstacles) || isOverlapping(x, y, collectables) || isOverlapping(x, y, goal);
		} while (overlapping);

		Point powerup;
		powerup.x = x;
		powerup.y = y;
		powerups.push_back(powerup);
	}
}
std::vector<Star> stars;

void initialize() {
	player.x = playerPositionx;
	player.y = playerPositiony;

	// Seed the random number generator with the current time
	srand(static_cast<unsigned int>(time(nullptr)));
	initializeObstacles();
	initializeCollectables();
	initializeGoal();
	initializePowerups();

	// Create a starry background
	for (int i = 0; i < 100; i++) {
		Star star;
		star.x = rand() % windowWidth;
		star.y = rand() % windowHeight;
		star.speed = (rand() % 5 + 1) / 10.0f; // Varying star speeds
		stars.push_back(star);
	}
}



/* collectable and powerups logic*/
bool isCollision(float playerX, float playerY, float collectableX, float collectableY, float radius) {
	float distance = sqrt((playerX - collectableX) * (playerX - collectableX) + (playerY - collectableY) * (playerY - collectableY));
	return distance < radius;
}

void ShieldOff(int value) {
	shieldOn = false;
}

void checkPowerupCollisions() {
	float playerRadius = 45.0f;  // Adjust the player's radius as needed

	for (size_t i = 0; i < powerups.size(); i++) {
		float collectableX = powerups[i].x;
		float collectableY = powerups[i].y;

		if (isCollision(player.x, player.y, collectableX, collectableY, playerRadius)) {
			if (i % 2 == 0) {
				shieldOn = true;
				glutTimerFunc(10000, ShieldOff, 0);

			}
			else {
				currentTime += 10000;
			}
			powerups.erase(powerups.begin() + i); // Remove the collected collectable
			i--; // Decrement the index to account for the removed collectable
		}
	}
}


void checkCollectableCollisions() {
	float playerRadius = 45.0f;  // Adjust the player's radius as needed

	for (size_t i = 0; i < collectables.size(); i++) {
		float collectableX = collectables[i].x;
		float collectableY = collectables[i].y;

		if (isCollision(player.x, player.y, collectableX, collectableY, playerRadius)) {
			score += 10; // You can adjust the score increment as needed
			collectables.erase(collectables.begin() + i); // Remove the collected collectable
			i--; // Decrement the index to account for the removed collectable
		}
	}
}



bool isObstacleCollision(float playerX, float playerY, float obstacleX, float obstacleY, float playerRadius, float obstacleWidth, float obstacleHeight) {
	float closestX = fmaxf(obstacleX - obstacleWidth / 2, fminf(playerX, obstacleX + obstacleWidth / 2));
	float closestY = fmaxf(obstacleY - obstacleHeight / 2, fminf(playerY, obstacleY + obstacleHeight / 2));
	float distanceX = playerX - closestX;
	float distanceY = playerY - closestY;
	float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
	return distanceSquared < (playerRadius * playerRadius);
}

void checkObstacleCollisions() {
	float playerRadius = 45.0f;  // Adjust the player's radius as needed

	for (size_t i = 0; i < obstacles.size(); i++) {
		float obstacleX = obstacles[i].x;
		float obstacleY = obstacles[i].y;
		float obstacleWidth = 50.0;  // Adjust obstacle dimensions as needed
		float obstacleHeight = 50.0;

		if (isObstacleCollision(player.x, player.y, obstacleX, obstacleY, playerRadius, obstacleWidth, obstacleHeight)) {
			// Collision detected, the player has hit an obstacle
			// Add your code to handle the collision here
			// For example, decrement lives and remove the obstacle
			lives--;  // You can adjust the number of lives lost as needed
			player.x = playerPositionx;
			player.y = playerPositiony;
		}
	}
}





void ScoreAndTime() {
	// Convert the score and game time to strings
	std::string scoreText = "Score: " + std::to_string(score);
	std::string timeText = "Time: " + std::to_string(currentTime / 1000) + "s";

	// Set the position for displaying the score and game time
	float scoreX = 2.2 * (windowWidth / 4); // Adjust the X position
	float scoreY = windowHeight - 30.0f; // Adjust the Y position
	float timeX = 2.2 * (windowWidth / 4);  // Adjust the X position
	float timeY = windowHeight - 60.0f; // Adjust the Y position

	// Display the score and game time using the drawText function
	drawText(scoreX, scoreY, scoreText);
	drawText(timeX, timeY, timeText);
}

void update(int value) {
	if (!gameOver) {
		currentTime -= 1000; // Decrement by 1 second (1000 milliseconds)
		if (currentTime <= 0) {
			currentTime = 0; // Ensure it doesn't go negative
			gameOver = true; // Game over when the timer reaches 0
		}

		// Request a redisplay to update the displayed time
		glutPostRedisplay();
	}

	// Set up the timer to call this function again in 1 second if the game is not over
	if (!gameOver && currentTime > 0) {
		glutTimerFunc(1000, update, 0);
	}
}


void animation(int value) {
	
	// Move stars
	for (size_t i = 0; i < stars.size(); i++) {
		stars[i].x -= stars[i].speed;

		// Reset stars when they move off-screen
		if (stars[i].x < 0) {
			stars[i].x = windowWidth;
			stars[i].y = rand() % windowHeight;
			stars[i].speed = (rand() % 5 + 1) / 10.0f; // Varying star speeds
		}
	}
	
	collectableRotation += 5.0f; // Adjust the rotation speed

	powerUpVerticalOffset = 10.0f * sin(collectableRotation * 3.14 / 180);
	// Update the rotation angle for animation
	if (goalrotateRight) {
		goalrotationAngle += goalrotationSpeed;
		if (goalrotationAngle > 10.0f) { // Adjust the angle range as needed
			goalrotateRight = false;
		}
	}
	else {
		goalrotationAngle -= goalrotationSpeed;
		if (goalrotationAngle < -10.0f) { // Adjust the angle range as needed
			goalrotateRight = true;
		}
	}



	glutPostRedisplay();

	// Set up the timer to call this function again in 10 seconds
	glutTimerFunc(100, animation, 0);

}

void displayEndGameScreen() {
	glClear(GL_COLOR_BUFFER_BIT);

	if (win) {
		// Display a message for winning the game
		glColor3f(0.0, 1.0, 0.0); // Green color
		drawText(windowWidth / 2 - 100, windowHeight / 2, "You Won!");
		drawText(windowWidth / 2 - 100, windowHeight / 2 - 100, "Score : " + std::to_string(score));
	}
	else {
		// Display a message for losing the game
		glColor3f(1.0, 0.0, 0.0); // Red color
		drawText(windowWidth / 2 - 100, windowHeight / 2, "You Lost!");
	}

	glFlush();
}









void Display() {
	glClear(GL_COLOR_BUFFER_BIT);

	if (gameOver)
		displayEndGameScreen();
	else {
		drawPlayer();
		drawObstacles();
		
		drawBoundaries();
		drawCollectables();
		HealthBar();
		drawGoal(goalpoint.x, goalpoint.y, 100, 70, goalrotationAngle);
		drawPowerUps();
		ScoreAndTime();
		// Draw the starry background
		glColor3f(1.0, 1.0, 1.0); // White color
		for (size_t i = 0; i < stars.size(); i++) {
			glPointSize(1.0); // Adjust point size as needed
			glBegin(GL_POINTS);
			glVertex2f(stars[i].x, stars[i].y);
			glEnd();
		}


	}



	glFlush();
}

bool isCollisionWithGoal(float playerX, float playerY, float goalX, float goalY) {
	// Define the rocket's characteristics
	float rocketWidth = 100; // Adjust rocket dimensions as needed
	float rocketHeight = 100;

	// Calculate the boundaries of the player (rocket) and goal
	float playerLeft = playerX - rocketWidth / 2;
	float playerRight = playerX + rocketWidth / 2;
	float playerTop = playerY + rocketHeight / 2;
	float playerBottom = playerY - rocketHeight / 2;

	// Define the goal's characteristics
	float goalWidth = 100; // Adjust goal dimensions as needed
	float goalHeight = 70;

	// Calculate the boundaries of the goal
	float goalLeft = goalX - goalWidth / 2;
	float goalRight = goalX + goalWidth / 2;
	float goalTop = goalY + goalHeight / 2;
	float goalBottom = goalY - goalHeight / 2;

	// Check for collision by comparing the boundaries
	if (playerRight < goalLeft || playerLeft > goalRight || playerTop < goalBottom || playerBottom > goalTop) {
		// No collision
		return false;
	}
	else {
		// Collision detected
		return true;
	}
}



void Move(unsigned char key, int x, int y) {

	if (key == 'a') {
		if (player.x > 50) {
			player.x -= 10;
			playerRotationAngle = 90.0;
		}
		else {
			if (!shieldOn) {
				player.x = player.x + 30;
				lives -= 1;
			}
		}
	}
	if (key == 'd') {
		if (player.x < windowWidth - 50) {
			player.x += 10;
			playerRotationAngle = -90.0;
		}
		else {
			if (!shieldOn) {
				player.x = player.x - 30;
				lives -= 1;
			}
		}
	}
	if (key == 's') {
		if (player.y > 40) {
			player.y -= 10;
			playerRotationAngle = 180.0;
		}
		else {
			if (!shieldOn) {
				player.y = player.y + 30;
				lives -= 1;
			}
		}
	}
	if (key == 'w') {
		if (player.y < windowHeight - 40) {
			player.y += 10;
			playerRotationAngle = 0.0;
		}
		else {
			if (!shieldOn) {
				player.y = player.y - 30;
				lives -= 1;
			}
		}
	}


	checkCollectableCollisions();
	if (!shieldOn)
		checkObstacleCollisions();
	checkPowerupCollisions();
	if (isCollisionWithGoal(player.x, player.y, goalpoint.x, goalpoint.y)) {
		gameOver = true;
		win = true;
	}


	if (lives == 0)
		gameOver = true;

	glutPostRedisplay();
}




void main(int argc, char** argr) {
	glutInit(&argc, argr);

	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(50, 50);


	glutCreateWindow("Game");
	initialize();
	glutTimerFunc(10000, update, 0);
	glutTimerFunc(10, animation, 0);

	glutDisplayFunc(Display);
	glutKeyboardFunc(Move);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glClearColor(0, 0, 0, 0);
	gluOrtho2D(0, windowWidth, 0, windowHeight);

	glutMainLoop();
}