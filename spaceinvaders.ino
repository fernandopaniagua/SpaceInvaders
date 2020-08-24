/*************
  Librerías 5110
**************/
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

//Inicialización LCD
Adafruit_PCD8544 display = Adafruit_PCD8544(13, 12, 11, 10, 9);

struct Alien {
  int x;
  int y;
  bool alive = true;
};

struct Shoot {
  int x;
  int y;
  bool active = false;
};

//ALIENS
const int ROWS = 2;//Numero de filas de aliens
const int COLS = 5;//Numero de columnas de aliens
const int NUM_ALIENS = ROWS * COLS;//Numero de alines
const int POINTS_PER_ALIEN = 10;//Puntos que proporciona cada Alien
const int NUM_CICLES = 2;//Numero de ciclos entre movimimientos de los alien

//Umbrales del Joystick
const int X_CENTER = 320;
const int OFFSET = 100;
const int RIGHT_OFFSET = X_CENTER + OFFSET;
const int LEFT_OFFSET = X_CENTER - OFFSET;

//PLAYER
const int PLAYER_SPEED = 2;

//BOTONES
const byte PIN_ANALOG_X = 0;//Derecha o izquierda en el pad analogico
const int E_BTN = 6;//Boton Start (E)
const int B_BTN = 3;//Boton Fire (B)

int score = 0;//Puntuación
int xJoy;//Entre 0 y 667

//PLAYER
int xPos = 36;
int yPos = 0;
int playerDirection = 0;//Direccion del player
Shoot shoot;//Displaro del player

//ALIEN
Alien aliens[NUM_ALIENS];
int enemyDirection = 1;//Direccion del enemigo
int cicles = 0;//Numero de ciclos que han pasado desde el ultimo movimiento del alien 
byte aliensSpeed = 1;//Velocidad de los alien
short aliensDestroyed = 0;//Numero de aliens destruidos

void setup() {
  Serial.begin(9600);
  display.begin();
  //Setup LCD
  display.setContrast(60);
  display.clearDisplay();
  initGame();
}

void loop() {
  if (score == NUM_ALIENS * POINTS_PER_ALIEN) {
    initGame();
  }
  if (digitalRead(B_BTN) == 0) {
    fire();
  }
  readJoystick();
  if (xJoy > RIGHT_OFFSET) {
    playerDirection = 1;
  } else if (xJoy < LEFT_OFFSET) {
    playerDirection = -1;
  } else {
    playerDirection = 0;
  }
  xPos += playerDirection * PLAYER_SPEED;
  if (xPos <= 1) xPos = 1;
  if (xPos >= 70) xPos = 70;
  drawPlayer();
  moveAliens();
  drawAliens();
  drawFire();
  drawScore();
  display.display();
  display.clearDisplay();
}

/**
   Inicializa el juego
*/
void initGame() {
  score = 0;
  aliensSpeed = 1;
  aliensDestroyed = 0;
  enemyDirection = 1;
  cicles = 0;
  shoot.active = false;
  spawnAliens();
  showIntroScreen();
}


/**
   Activa el disparo en el punto de disparo del player
*/
void fire() {
  if (!shoot.active) {
    shoot.x = xPos + 6;
    shoot.y = 38;
    shoot.active = true;
  }
}

/**
   Dibuja el disparo del player
*/
void drawFire() {
  if (shoot.active) {
    line(shoot.x, shoot.y, shoot.x, shoot.y + 2);
    shoot.y--;
    if (shoot.y == 0) shoot.active = false;
    onCollisionEnter();
  }
}

/**
   Detecta la colision con los aliens
*/
void onCollisionEnter() {
  for (int i = 0; i < NUM_ALIENS; i++) {
    if (aliens[i].alive == true) {
      if ((shoot.x > aliens[i].x) && (shoot.x < aliens[i].x + 11) && (shoot.y > aliens[i].y) && (shoot.y < aliens[i].y + 8)) {
        aliens[i].alive = false;
        aliensDestroyed++;
        if (aliensDestroyed == 5) aliensSpeed = 2;
        shoot.active = false;
        score += POINTS_PER_ALIEN;
        break;
      }
    }
  }
}

/**
   Dibuja el player...
*/
void drawPlayer() {
  line(xPos + 6, 41, xPos + 6, 41);
  line(xPos + 5, 42, xPos + 7, 42);
  line(xPos + 5, 43, xPos + 7, 43);
  line(xPos + 1, 44, xPos + 11, 44);
  line(xPos, 45, xPos + 12, 45);
  line(xPos, 46, xPos + 12, 46);
  line(xPos, 47, xPos + 12, 47);
}

void moveAliens() {
  cicles++;
  if (cicles == NUM_CICLES) {
    for (int i = 0; i < NUM_ALIENS; i++) {
      if (aliens[i].alive == true) {
        aliens[i].x = aliens[i].x + enemyDirection * aliensSpeed;
      }
    }
    cicles = 0;
  }
}

void spawnAliens() {
  int idAlien = 0;
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      aliens[idAlien].x = j * 14 + 1;
      aliens[idAlien].y = i * 10 + 8;
      aliens[idAlien].alive = true;
      idAlien++;
    }
  }
}

/**
 * Dibujo de los aliens
 */
void drawAliens() {
  bool changeDirection = false;
  for (int i = 0; i < NUM_ALIENS; i++) {
    if (aliens[i].alive == true) {
      line(aliens[i].x + 3, aliens[i].y + 1, aliens[i].x + 3, aliens[i].y + 1);
      line(aliens[i].x + 9, aliens[i].y + 1, aliens[i].x + 9, aliens[i].y + 1);
      line(aliens[i].x + 4, aliens[i].y + 2, aliens[i].x + 4, aliens[i].y + 2);
      line(aliens[i].x + 8, aliens[i].y + 2, aliens[i].x + 8, aliens[i].y + 2);
      line(aliens[i].x + 3, aliens[i].y + 3, aliens[i].x + 9, aliens[i].y + 3);
      line(aliens[i].x + 2, aliens[i].y + 4, aliens[i].x + 3, aliens[i].y + 4);
      line(aliens[i].x + 5, aliens[i].y + 4, aliens[i].x + 7, aliens[i].y + 4);
      line(aliens[i].x + 9, aliens[i].y + 4, aliens[i].x + 10, aliens[i].y + 4);
      line(aliens[i].x + 1, aliens[i].y + 5, aliens[i].x + 11, aliens[i].y + 5);
      line(aliens[i].x + 1, aliens[i].y + 6, aliens[i].x + 1, aliens[i].y + 6);
      line(aliens[i].x + 3, aliens[i].y + 6, aliens[i].x + 9, aliens[i].y + 6);
      line(aliens[i].x + 11, aliens[i].y + 6, aliens[i].x + 11, aliens[i].y + 6);
      line(aliens[i].x + 1, aliens[i].y + 7, aliens[i].x + 1, aliens[i].y + 7);
      line(aliens[i].x + 3, aliens[i].y + 7, aliens[i].x + 3, aliens[i].y + 7);
      line(aliens[i].x + 9, aliens[i].y + 7, aliens[i].x + 9, aliens[i].y + 7);
      line(aliens[i].x + 11, aliens[i].y + 7, aliens[i].x + 11, aliens[i].y + 7);
      line(aliens[i].x + 4, aliens[i].y + 8, aliens[i].x + 5, aliens[i].y + 8);
      line(aliens[i].x + 7, aliens[i].y + 8, aliens[i].x + 8, aliens[i].y + 8);
      if (aliens[i].x <= 0) {
        if (!changeDirection) {
          enemyDirection = 1 * aliensSpeed;
          changeDirection = true;
        }
      }
      if (aliens[i].x >= 72) {
        if (!changeDirection) {
          enemyDirection = -1 * aliensSpeed;
          changeDirection = true;
        }
      }
    }
  }
  if (changeDirection) {
    bool gameover = false;
    for (int i = 0; i < NUM_ALIENS; i++) {
      if (aliens[i].alive == true) {
        aliens[i].y++;
        if (aliens[i].y + 8 > 41) {
          gameover=true;
        }
      }
    }
    if (gameover) showGameOverScreen();
  }
}

/**
 * Lectura del joystick
 */
void readJoystick() {
  //Lectura del joystick
  xJoy = analogRead(PIN_ANALOG_X);
  delay(50);//Pausa necesaria para no duplicar lecturas
}

/**
   Pinta una línea recta.
   Parámetros:
   x0 - Coordeanda x del punto de inicio de la recta
   y0 - Coordenada y del punto de inicio de la recta
   x1 - Coordenada x del punto de final de la recta
   y1 - Coordenada y del punto de final de la recta
*/
void line(int x0, int y0, int x1, int y1) {
  display.drawFastHLine(x0, y0, x1 - x0 + 1, BLACK);
}
/**
   Pinta la puntuación y el número de vidas
*/
void drawScore() {
  display.setCursor(0, 0);
  display.print("SCORE ");
  display.print(score);
}

/**
   Muestra la pantalla de presentación
*/
void showIntroScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.println("By F. Paniagua");
  display.println("");
  display.println("SPACE INVADERS");
  display.println("     2020     ");
  display.println("");
  display.println("-PRESS  START-");
  display.display();
  while (digitalRead(E_BTN) != 0);
}

/**
   Muestra game over
*/
void showGameOverScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.println("");
  display.println("");
  display.println("  GAME  OVER  ");
  display.println("");
  display.println("");
  display.println("-PRESS  START-");
  display.display();
  while (digitalRead(E_BTN) != 0);
  delay(100);//Para no duplicar la lectura del boton Start (E)
  initGame();
}
