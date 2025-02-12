#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h> 
#include <string.h>

// Web
#define IS_WEB true

// Constants
#define NO_BUTTON -1  // Add this constant for button states

// Render resolution
#define BASE_SCREEN_WIDTH 1920
#define BASE_SCREEN_HEIGHT 1080

// Debug flags
#define DEBUG_SHOW false
#define DEBUG_FASTLOAD true
#define DEBUG_MAX_FPS_HISTORY 500
#define DEBUG_MAX_LOGS_HISTORY 25

// Base values
const float baseX = -(BASE_SCREEN_WIDTH / 2);
const float baseY = -(BASE_SCREEN_HEIGHT / 2);
const float targetAspectRatio = (float)BASE_SCREEN_WIDTH / (float)BASE_SCREEN_HEIGHT;
const int targetFps = 300;
const float bgmVolume = 0.25f;
const float gameDuration = 60.0f * 2.0f;

// Debug FPS history
int DebugFpsHistory[DEBUG_MAX_FPS_HISTORY];
int DebugFpsHistoryIndex = 0;

// Debug frame time history
int DebugFrameTimeHistory[DEBUG_MAX_FPS_HISTORY];
int DebugFrameTimeHistoryIndex = 0;

// Debug logs history
typedef struct DebugLogEntry {
	int type;
	char* text;
} DebugLogEntry;

DebugLogEntry DebugLogs[DEBUG_MAX_LOGS_HISTORY];
int DebugLogsIndex = 0;

// Debug tool toggles states
typedef struct DebugToolToggles {
	bool showDebugLogs;
	bool showStats;
	bool showGraph;
    bool showObjects;
} DebugToolToggles;

DebugToolToggles debugToolToggles = { false, true, false, false };

void LogDebug(const char* text, ...);
void Log(int msgType, const char* text, ...);

// Runtime resolution
typedef struct Resolution {
    int x;
    int y;
} Resolution;

// Difficulty
typedef enum {
    EASY,
    MEDIUM,
    HARD,
    FREEPLAY_EASY,
    FREEPLAY_MEDIUM,
    FREEPLAY_HARD
} Difficulty;

// Game options
typedef struct GameOptions {
    Resolution resolution;
    int targetFps;
    bool fullscreen;
    Difficulty difficulty;
    bool showDebug;
    bool soundFxEnabled;
    bool musicEnabled;
} GameOptions;

// Colors
Color MAIN_BROWN = { 150, 104, 81, 255 };
Color MAIN_ORANGE = { 245, 167, 128, 255 };

// Textures
Texture2D logoTexture;
Texture2D splashBackgroundTexture;
Texture2D splashOverlayTexture;
Texture2D backgroundTexture;
Texture2D backgroundOverlayTexture;
Texture2D backgroundOverlaySidebarTexture;
Texture2D pawTexture;

// UI Elements
Texture2D checkbox;
Texture2D checkboxChecked;
Texture2D left_arrow;
Texture2D right_arrow;

// Customer
Texture2D customerTexture_first_happy;
Texture2D customerTexture_second_happy;
Texture2D customerTexture_third_happy;
Texture2D customerTexture_first_normal;
Texture2D customerTexture_second_normal;
Texture2D customerTexture_third_normal;
Texture2D customerTexture_first_angry;
Texture2D customerTexture_second_angry;
Texture2D customerTexture_third_angry;
Texture2D bubbles;

// Clouds
Texture2D cloud1Texture;
Texture2D cloud2Texture;
Texture2D cloud3Texture;

// Stars
Texture2D star1Texture;
Texture2D star2Texture;

// Font
Font meowFont;

// Sounds
Sound selectFx;
Sound hoverFx;
Sound boongFx;

Sound angry1Fx;
Sound angry2Fx;
Sound angry3Fx;
Sound angry4Fx;
      
Sound bottle1Fx;
Sound bottle2Fx;
Sound bottle3Fx;

Sound confused1Fx;
Sound confused2Fx;
Sound confused3Fx;
Sound confused4Fx;

Sound correctFx;

Sound drop1Fx;
Sound drop2Fx;
Sound drop3Fx;

Sound pickup1Fx;
Sound pickup2Fx;
Sound pickup3Fx;

Sound pour1Fx;
Sound pour2Fx;
Sound pour3Fx;

Sound stir1Fx;
Sound stir2Fx;
Sound stir3Fx;

Sound flickFx;

typedef enum {
    FX_HOVER,
    FX_SELECT,
    FX_ANGRY,
    FX_BOTTLE,
    FX_CONFUSED,
    FX_CORRECT,
    FX_DROP,
    FX_PICKUP,
    FX_POUR,
    FX_STIR,
    FX_BOONG,
    FX_FLICK
} SoundFxType;

// BGMs
Music menuBgm;

// Menu falling items
Texture2D menuFallingItemTextures[8];

// Ingredients
Texture2D teaPowderTexture;
Texture2D cocoaPowderTexture;
Texture2D caramelSauceTexture;
Texture2D chocolateSauceTexture;
Texture2D condensedMilkTexture;
Texture2D normalMilkTexture;
Texture2D marshMellowTexture;
Texture2D whippedCreamTexture;
Texture2D hotWaterTexture;
Texture2D greenChonTexture;
Texture2D cocoaChonTexture;


// utils
Texture2D trashCanTexture;

static inline char* StringFromDifficultyEnum(Difficulty difficulty)
{
    static const char* strings[] = { "Easy", "Medium", "Hard", "Freeplay (E)", "Freeplay (M)", "Freeplay (H)" };
    return strings[difficulty];
}

// Customer textures
typedef struct {
    Texture2D happy;
    Texture2D happyEyesClosed;
    Texture2D frustrated;
    Texture2D frustratedEyesClosed;
    Texture2D angry;
    Texture2D angryEyesClosed;
} CustomerImageData;

CustomerImageData customersImageData[3];

typedef enum {
    TEXTURE_TYPE_HAPPY,
    TEXTURE_TYPE_HAPPY_EYES_CLOSED,
    TEXTURE_TYPE_FRUSTRATED,
    TEXTURE_TYPE_FRUSTRATED_EYES_CLOSED,
    TEXTURE_TYPE_ANGRY,
    TEXTURE_TYPE_ANGRY_EYES_CLOSED
} CustomerTextureFrameType;

// Customer emotions
typedef enum {
    EMOTION_HAPPY,
    EMOTION_FRUSTRATED,
    EMOTION_ANGRY
} CustomerEmotion;

// Order
// typedef struct Order { //text-based-combinations
//     char* first;
//     char* second;
//     char* third;
//     char* fourth;
// } Order;

// Customer
typedef struct Customer {
    CustomerEmotion emotion;
    double blinkTimer;
    double normalDuration;
    double blinkDuration;
    bool eyesClosed;

    //int patience; //To be removed. 
    bool visible;
    char order[20];
    double currentTime;
    int orderEnd;
    Vector2 position;
    int textureType;
    bool isDummy;
    double resetTimer;
} Customer;

static inline char* StringFromCustomerEmotionEnum(CustomerEmotion emotion)
{
    static const char* strings[] = { "Happy", "Frustrated", "Angry" };
    return strings[emotion];
}

double GetRandomDoubleValue(double min, double max)
{
    return min + (rand() / (double)RAND_MAX) * (max - min);
}

void RandomCustomerBlinkTime(Customer* customer) {
    customer->blinkDuration = GetRandomDoubleValue(0.2, 0.5);
    customer->normalDuration = GetRandomDoubleValue(2.0, 6.0);
}

Customer CreateCustomer(CustomerEmotion emotion, double blinkTimer, double normalDuration, double blinkDuration, bool visible, Vector2 position, int textureType, double resetTimer) {
    Customer newCustomer;

    newCustomer.emotion = emotion;
    newCustomer.blinkTimer = blinkTimer;
    newCustomer.normalDuration = normalDuration;
    newCustomer.blinkDuration = blinkDuration;
    newCustomer.eyesClosed = false;
    newCustomer.visible = visible;
	newCustomer.order[0] = '\0';
    newCustomer.currentTime = 0;
    newCustomer.orderEnd = 0;
    newCustomer.position = position;
    newCustomer.textureType = textureType;
    newCustomer.isDummy = false;
    newCustomer.resetTimer = resetTimer;

    return newCustomer;
}

// Customers
typedef struct Customers {
    Customer customer1;
    Customer customer2;
    Customer customer3;
} Customers;

// Menu falling items
typedef struct {
    Vector2 position;
    float rotation;
    int textureIndex;
    float fallingSpeed;
    float rotationSpeed;
} MenuFallingItem;

// Moving clouds
typedef struct {
	Vector2 position;
	float speed;
	float scale;
	Texture2D texture;
    bool fromRight;
} MovingCloud;

// Moving stars
typedef struct {
	Vector2 position;
	float speed;
	float scale;
	Texture2D texture;
} MovingStar;

// Ingredient
typedef enum IngredientType {
    NONE,
    GREEN_TEA,
    COCOA,
    CONDENSED_MILK,
    MILK,
    MARSHMELLOW,
    WHIPPED_CREAM,
    CARAMEL,
    CHOCOLATE
};

typedef struct {
    Texture2D texture;
    bool canChangeCupTexture;
    Vector2 position;
    Vector2 originalPosition;
    Rectangle frameRectangle;
    int totalFrames;
    int currentFrame; // not use right now but later
} Ingredient;

// Ingredients
Ingredient teaPowder, cocoaPowder, normalMilk, condensedMilk, marshMellow, whippedCream, caramelSauce, chocolateSauce, hotWater;
Ingredient greenChon, cocoaChon;

Ingredient trashCan;

// Cup
typedef struct {
    Texture2D texture;
    Vector2 position;
    Vector2 originalPosition;
    Rectangle frameRectangle;
    // Add properties to represent cup state
    enum IngredientType powderType;
    bool hasWater;
    enum IngredientType creamerType;
    enum IngredientType toppingType;
    enum IngredientType sauceType;
    bool active;
    char* order[20];
} Cup;

// Drop area
typedef struct {
    /* data */
    Texture2D texture;
    Vector2 position;
} DropArea;

DropArea plate;

// Original position
const Vector2 oricupPosition = { 351,109 };
const Vector2 oriwaterPosition = { 600, 210 };

const Vector2 oricaramelPosition = { -770,108 };
const Vector2 orichocolatePosition = { -904,138 };
const Vector2 oriteapowderPosition = { -421,145 };
const Vector2 oricocoapowderPosition = { -564,177 };
const Vector2 oriplatePosition = { -175,300 };
const Vector2 oriplateCupPosition = { -28, 300 };
const Vector2 oricondensedmilkPosition = { 283,316 };
const Vector2 orimilkPosition = { 160,342 };
const Vector2 orimarshmellowPosition = { -481,320 };
const Vector2 oriwhippedPosition = { -644,320 };
const Vector2 oricupsPostion = { 390,80 };
const Vector2 hiddenPosition = { -3000, -3000 };
Vector2 trashCanPosition = { 0, 0 };

static int global_score = 0;

bool triggerHotWater = false;
double boilingTime = 2.5;
double lastBoongBoongBoongTime = 0;
bool dragAndDropLocked = false;

void UnloadGlobalAssets();
void ExitApplication()
{
    UnloadGlobalAssets();
    exit(0);
}

void boilWater(Ingredient* item) {
    if (!item->canChangeCupTexture) {
        triggerHotWater = true;
        boilingTime = GetTime();
    }
}

void PlaySoundFx(SoundFxType type);
void RemoveCustomer(Customer* customer);
bool validiator(Customer* customer, char* order);
Texture2D* DragAndDropCup(Cup* cup, const DropArea* dropArea, Camera2D* camera, Customers *customers, Ingredient* trashCan)
{
    static bool isObjectBeingDragged = false;
    static Texture2D* current_dragging = NULL;
    static float offsetX = 0;
    static float offsetY = 0;

    Rectangle objectBounds = { cup->position.x, cup->position.y, (float)cup->frameRectangle.width, (float)cup->frameRectangle.height };
    Rectangle dropBounds = { dropArea->position.x, dropArea->position.y, (float)dropArea->texture.width, (float)dropArea->texture.height };

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), *camera);

        Rectangle trashCanBond = { trashCan->position.x, trashCan->position.y, trashCan->frameRectangle.width, trashCan->frameRectangle.height };
        LogDebug("trashCanPosition.x: %f, trashCanPosition.y: %f, trashCanTexture.width: %d, trashCanTexture.height: %d\n", trashCanPosition.x, trashCanPosition.y, trashCanTexture.width, trashCanTexture.height);
        Rectangle cupBond = { cup->position.x, cup->position.y, cup->texture.width, cup->texture.height };
        LogDebug("Cup Positon x: %f, y: %f, cupTexture.width: %d, cupTexture.height: %d\n", cup->position.x, cup->position.y, cup->texture.width, cup->texture.height);
        LogDebug("CheckCollisionRecs(trashCanBond, cupBond) %d\n", CheckCollisionRecs(trashCanBond, cupBond));
        if(CheckCollisionRecs(trashCanBond, cupBond)){
            cup->powderType = NONE;
            cup->creamerType = NONE;
            cup->toppingType = NONE;
            cup->sauceType = NONE;
            cup->hasWater = false;
            cup->active = false;
        }

        if (cup->active && CheckCollisionPointRec(mousePos, objectBounds) && (current_dragging == NULL || current_dragging == &cup->texture)) {
            isObjectBeingDragged = true;
            offsetX = cup->frameRectangle.width / 2;
            offsetY = cup->frameRectangle.height / 2;
            float mouseX = mousePos.x;
            float mouseY = mousePos.y;

            cup->position.x = mouseX - offsetX;
            cup->position.y = mouseY - offsetY;
            current_dragging = &cup->texture;
            return &cup->texture;
        }
        // If being drag from cups, then change the cup position to the cursor
        else if (!cup->active && CheckCollisionPointRec(mousePos, (Rectangle) { oricupsPostion.x, oricupsPostion.y, cup->texture.width, cup->texture.height })) {
			// Reset cup state            
            isObjectBeingDragged = true;
			offsetX = cup->texture.width / 2;
			offsetY = cup->texture.height / 2;
			float mouseX = mousePos.x;
			float mouseY = mousePos.y;

			cup->position.x = mouseX - offsetX;
			cup->position.y = mouseY - offsetY;
            cup->texture = LoadTexture(ASSETS_PATH"combination/EMPTY.png");
			current_dragging = &cup->texture;
            cup->powderType = NONE;
            cup->creamerType = NONE;
            cup->toppingType = NONE;
            cup->sauceType = NONE;
            cup->hasWater = false;
            cup->active = true;
			return &cup->texture;
		}

    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        isObjectBeingDragged = false;
        current_dragging = NULL;
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), *camera);

        Customer* customerToCheck[3] = { &customers->customer1, &customers->customer2, &customers->customer3, };

        
        Rectangle cupRect = { cup->position.x, cup->position.y, cup->frameRectangle.width, cup->frameRectangle.height };

        // Only check if world mouse position is more than y= 15
        if (mousePos.y <= 15 && cup->active)
        {
            // If cup is being given to customers
            for (int i = 0; i < 3; i++) {
                // If customer is visible
                if (customerToCheck[i]->visible) {
                    // If cup is being given to customer

                    Rectangle customerRect = { customerToCheck[i]->position.x, customerToCheck[i]->position.y, customersImageData[i].happy.width / 2, customersImageData[i].happy.height / 2 };

                if (cupRect.x >= customerRect.x &&
                    cupRect.y >= customerRect.y &&
                    (cupRect.x + cupRect.width) <= (customerRect.x + customerRect.width) &&
                    (cupRect.y + cupRect.height) <= (customerRect.y + customerRect.height))
                {
                        bool correct = validiator(customerToCheck[i], cup->order);
                        if (correct)
                        {
                            global_score += 50;
                            PlaySoundFx(FX_CORRECT);

                            customerToCheck[i]->visible = false;
                            RemoveCustomer(customerToCheck[i]);
                        }
                        else
                        {
                            global_score -= 50;
                            PlaySoundFx(FX_CONFUSED);
                        }

                        // Reset cup state
                        cup->texture = LoadTexture(ASSETS_PATH"combination/EMPTY.png");
                        cup->powderType = NONE;
                        cup->creamerType = NONE;
                        cup->toppingType = NONE;
                        cup->sauceType = NONE;
                        cup->hasWater = false;
                        cup->active = false;
                        return &cup->texture;
                    }
                }
            }
        }


        // Return cup to the drop area if it is not inside the drop area
        int offset_x = 10;
        int offset_y = -40;

        cup->position.x = dropArea->position.x + offset_x + dropArea->texture.width / 2 - cup->frameRectangle.width / 2;
        cup->position.y = dropArea->position.y + offset_y + dropArea->texture.height / 2 - cup->frameRectangle.height / 2;

        //if (CheckCollisionRecs(objectBounds, dropBounds)) {
            // center of cup to center of drop area
       
       // }
       //else {
            //cup->position.x = cup->originalPosition.x;
            //cup->position.y = cup->originalPosition.y;
       // }

    }

    // Apply drag-and-drop with time-dependent positioning
    if (isObjectBeingDragged) {
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), *camera);
        cup->position.x = mousePos.x - offsetX;
        cup->position.y = mousePos.y - offsetY;
    }

    if(current_dragging)
        return current_dragging;
    else
        return NULL;

}

void UpdateCupImage(Cup* cup, Ingredient* ingredient) {
    // Check what type of ingredient it is and update the cup accordingly
    // if (ingredient == &teaPowder) {
    //     cup->hasPowder = true;
    //     cup->texture = LoadTexture(ASSETS_PATH"/combination/milktea.png"); // Change to the tea cup texture
    // } else if (ingredient == &cocoaPowder) {
    //     cup->hasPowder = true;
    //     cup->texture = LoadTexture(ASSETS_PATH"/combination/milkcocoa.png"); // Change to the cocoa cup texture
    // }

    // this is a naming standard for combination
    // {POWDER}{CREAMER}{TOPPING}{SAUCE}.png

    // POWDER:
    // -Cocoa Powder: CP
    // -Green Tea Powder: GP

    // WATER:
    // -Yes: Y
    // -No: N

    // CREAMER:
    // -Condensed Milk: CM
    // -Milk: MI

    // TOPPING:
    // -Marshmallow: MA
    // -Whipped Cream: WC

    // SAUCE:
    // -Caramel: CA
    // -Chocolate: CH

    LogDebug("update CUP IMAGE \n");
    char filename[100];

    // Initialize filename to empty string
    strcpy(filename, "");

    switch (cup->powderType)
    {
    case GREEN_TEA:
        // cup->texture = LoadTexture(ASSETS_PATH"/combination/milktea.png"); // Change to the tea cup texture
        strcat(filename, "GP");
        break;
    case COCOA:
        // cup->texture = LoadTexture(ASSETS_PATH"/combination/milkcocoa.png"); // Change to the cocoa cup texture
        strcat(filename, "CP");
        break;
    default:
        break;
    }

    if (cup->powderType != NONE) {
        switch (cup->hasWater)
        {
        case true:
            strcat(filename, "Y");
            break;
        case false:
            strcat(filename, "N");
            break;
        default:
            break;
        }
    }
    if (cup->hasWater == true) {
        switch (cup->creamerType)
        {
        case CONDENSED_MILK:
            strcat(filename, "CM");
            break;
        case MILK:
            strcat(filename, "MI");
            break;
        default:
            break;
        }
    }

    if (cup->creamerType != NONE) {
        switch (cup->toppingType)
        {
        case MARSHMELLOW:
            strcat(filename, "MA");
            break;
        case WHIPPED_CREAM:
            strcat(filename, "WC");
            break;
        default:
            break;
        }
    }

    if (cup->toppingType != NONE) {
        switch (cup->sauceType)
        {
        case CARAMEL:
            strcat(filename, "CA");
            break;
        case CHOCOLATE:
            strcat(filename, "CH");
            break;
        default:
            break;
        }
    }
    // if empty then set filename to EMPTY
    if (strcmp(filename, "") == 0) {
        strcat(filename, "EMPTY");
    }

    // Save filename to cup->order
    strcpy(cup->order, filename);

    strcat(filename, ".png");
    char path[1000];
    strcpy(path, ASSETS_PATH"combination/");
    strcat(path, filename);
    // set cup texture to the filename
    LogDebug("Powder type: %d, Water: %d, Creamer: %d, Topping: %d, Sauce: %d\n", cup->powderType, cup->hasWater, cup->creamerType, cup->toppingType, cup->sauceType);
    LogDebug("NEW CUP IMAGE IS %s\n", path);

    cup->texture = LoadTexture(path);
}

void UpdateCup(Cup* cup, Ingredient* ingredient) {
    // If cup is not active, return
    if (!cup->active) return;

    // Check what type of ingredient it is and update the cup accordingly
    if (ingredient == &teaPowder && cup->powderType == NONE) {
        cup->powderType = GREEN_TEA;
    }
    else if (ingredient == &cocoaPowder && cup->powderType == NONE) {
        cup->powderType = COCOA;
    }
    else if (ingredient == &hotWater && cup->powderType != NONE) {
        cup->hasWater = true;

        PlaySoundFx(FX_POUR);
        PlaySoundFx(FX_STIR);
    }
    else if (ingredient == &condensedMilk && cup->hasWater == true && cup->creamerType == NONE) {
        cup->creamerType = CONDENSED_MILK;
    }
    else if (ingredient == &normalMilk && cup->hasWater == true && cup->creamerType == NONE) {
        cup->creamerType = MILK;
        PlaySoundFx(FX_POUR);
    }
    else if (ingredient == &marshMellow && cup->creamerType != NONE && cup->toppingType == NONE) {
        cup->toppingType = MARSHMELLOW;
    }
    else if (ingredient == &whippedCream && cup->creamerType != NONE && cup->toppingType == NONE) {
        cup->toppingType = WHIPPED_CREAM;
    }
    else if (ingredient == &caramelSauce && cup->toppingType != NONE && cup->sauceType == NONE) {
        cup->sauceType = CARAMEL;
        PlaySoundFx(FX_BOTTLE);
    }
    else if (ingredient == &chocolateSauce && cup->toppingType != NONE && cup->sauceType == NONE) {
        cup->sauceType = CHOCOLATE;
        PlaySoundFx(FX_BOTTLE);
    }
    UpdateCupImage(cup, ingredient);

}

Texture2D* DragAndDropIngredient(Ingredient* object, Cup* cup, Camera2D* camera) {
    static bool isObjectBeingDragged = false;
    static Texture2D* current_dragging = NULL;
    static float offsetX = 0;
    static float offsetY = 0;

    Rectangle objectBounds = { object->position.x, object->position.y, (float)object->frameRectangle.width, (float)object->frameRectangle.height };
    Rectangle cupBounds = { cup->position.x, cup->position.y, (float)cup->texture.width, (float)cup->texture.height };

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), *camera);
        if (CheckCollisionPointRec(mousePos, objectBounds) && (current_dragging == NULL || current_dragging == &object->texture)) {

            if (object == &hotWater && object->canChangeCupTexture == false) {
                object->canChangeCupTexture = false;
                PlaySoundFx(FX_FLICK);
                boilWater(object);
            }

            else
            {
                isObjectBeingDragged = true;
                offsetX = object->frameRectangle.width / 2;
                offsetY = object->frameRectangle.height / 2;
                float mouseX = mousePos.x;
                float mouseY = mousePos.y;

                object->position.x = mouseX - offsetX;
                object->position.y = mouseY - offsetY;
                current_dragging = &object->texture;
                return &object->texture;
            }
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        isObjectBeingDragged = false;
        current_dragging = NULL;

        if (CheckCollisionRecs(objectBounds, cupBounds)) {
            if (object->canChangeCupTexture) {

                if (object == &hotWater && cup->powderType != NONE) {
                    triggerHotWater = false;
                    object->canChangeCupTexture = false;
                    object->currentFrame = 1;
                }

                UpdateCup(cup,object);

                object->position.x = object->originalPosition.x;
                object->position.y = object->originalPosition.y;
            }
        }
        else {
            // Object is not inside the drop area, return it to the original position
            object->position.x = object->originalPosition.x;
            object->position.y = object->originalPosition.y;
        }
    }

    // Apply drag-and-drop with time-dependent positioning
    if (isObjectBeingDragged) {
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), *camera);
        object->position.x = mousePos.x - offsetX;
        object->position.y = mousePos.y - offsetY;
    }

    if(current_dragging)
        return current_dragging;
	else
        return NULL;
}

Texture2D* DragAndDropIngredientPop(Ingredient* object, Ingredient* popObject, Cup* cup, Camera2D* camera) {
    static bool isObjectBeingDragged = false;
    static Texture2D* current_dragging = NULL;
    static float offsetX = 0;
    static float offsetY = 0;


    Rectangle objectBounds = { object->position.x, object->position.y, (float)object->frameRectangle.width, (float)object->frameRectangle.height };
    Rectangle popObjectBounds = { popObject->position.x, popObject->position.y, (float)popObject->frameRectangle.width, (float)popObject->frameRectangle.height };
    Rectangle cupBounds = { cup->position.x, cup->position.y, (float)cup->texture.width, (float)cup->texture.height };

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), *camera);
        if ((CheckCollisionPointRec(mousePos, objectBounds) || CheckCollisionPointRec(mousePos, popObjectBounds)) && (current_dragging == NULL || current_dragging == &object->texture)) {
            isObjectBeingDragged = true;
            offsetX = popObject->frameRectangle.width / 2;
            offsetY = popObject->frameRectangle.height / 2;
            float mouseX = mousePos.x;
            float mouseY = mousePos.y;

            popObject->position.x = mouseX - offsetX;
            popObject->position.y = mouseY - offsetY;
            current_dragging = &object->texture;
            return &object->texture;
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        isObjectBeingDragged = false;
        current_dragging = NULL;

        if (CheckCollisionRecs(popObjectBounds, cupBounds)) {
            if (object->canChangeCupTexture) {
                UpdateCup(cup,object);
                popObject->position.x = popObject->originalPosition.x;
                popObject->position.y = popObject->originalPosition.y;
            }
        }
        else {
            // Object is not inside the drop area, return it to the original position
            popObject->position.x = popObject->originalPosition.x;
            popObject->position.y = popObject->originalPosition.y;
        }
    }

    // Apply drag-and-drop with time-dependent positioning
    if (isObjectBeingDragged) {
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), *camera);
        popObject->position.x = mousePos.x - offsetX;
        popObject->position.y = mousePos.y - offsetY;
    }

    if(current_dragging)
	    return current_dragging;
    else
        return NULL;
}

Rectangle frameRect(Ingredient i, int frameNum, int frameToShow) {
    int frameWidth = i.texture.width / frameNum;
    Rectangle frameRect = { frameWidth * (frameToShow - 1), 0, frameWidth, i.texture.height };
    return frameRect;
}

Rectangle frameRectCup(Cup i, int frameNum, int frameToShow) {
    int frameWidth = i.texture.width / frameNum;
    Rectangle frameRect = { frameWidth * (frameToShow - 1), 0, frameWidth, i.texture.height };
    return frameRect;
}

void tickBoil(Ingredient* boiler) {

    if (triggerHotWater) {
        if (boilingTime + 3 > GetTime()) {
            boiler->currentFrame = boiler->totalFrames;
            return;
        }
        if (lastBoongBoongBoongTime + 0.5 < GetTime()) {
            StopSound(boongFx);
            PlaySoundFx(FX_BOONG);
            boiler->canChangeCupTexture = true;
            lastBoongBoongBoongTime = GetTime();
            int nextFrame = boiler->currentFrame + 2;
            if (nextFrame > boiler->totalFrames) {
                nextFrame = 1;
            }
            boiler->currentFrame = nextFrame;
        }
    }
}


bool highlightItem(Ingredient* item, Camera2D* camera) {
    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), *camera);
    static bool isHovering;
    if (CheckCollisionPointRec(mousePos, (Rectangle) { item->position.x, item->position.y, item->frameRectangle.width, item->frameRectangle.height }) && item->totalFrames > item->currentFrame) {
        item->frameRectangle = frameRect(*item, item->totalFrames, item->currentFrame + 1);
        return true;
    }
    else {
        item->frameRectangle = frameRect(*item, item->totalFrames, item->currentFrame);
        return false;
    }
}

// Function prototype
void MainMenuUpdate(Camera2D* camera, bool playFade);
void OptionsUpdate(Camera2D* camera);
void endgameUpdate(Camera2D* camera);

// Menu customers
Customer menuCustomer1;
Customer menuCustomer2;

// Menu falling items
#define menuFallingItemsNumber 8
MenuFallingItem menuFallingItems[20];
GameOptions *options;

// Moving clouds
MovingCloud movingClouds[7];

// Moving stars
MovingStar movingStars[2];

// Current BGM
Music* currentBgm = NULL;
bool isCurrentBgmPaused = false;

// Load duration timer
double loadDurationTimer = 0.0;
bool isGlobalAssetsLoadFinished = false;

// Start from night
int currentColorIndex = 3;
float dayNightCycleDuration = 120.0f;
// Skip to 1/2 of the night, so that the first transition is from night to morning
float colorTransitionTime = 0.5f;


void CustomLogger(int msgType, const char* text, va_list args)
{
    char timeStr[64] = { 0 };
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);

    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[%s] ", timeStr);

    // Calculate the size needed for the formatted log message
    int logMessageSize = vsnprintf(NULL, 0, text, args) + 1;
    char* logMessage = (char*)malloc(logMessageSize);

    switch (msgType)
    {
        case LOG_DEBUG: printf("[DEBUG]: "); break;
        case LOG_INFO: printf("[INFO] : "); break;
        case LOG_WARNING: printf("[WARN] : "); break;
        case LOG_ERROR: printf("[ERROR]: "); break;
        case LOG_FATAL: printf("[FATAL]: "); break;
        default: break;
    }

    // Format the log message and store it in logMessage
    vsnprintf(logMessage, logMessageSize, text, args);

    // Add message to the log array, with shifting logic
    if (DebugLogsIndex == DEBUG_MAX_LOGS_HISTORY) {
        // If the array is full, shift the previous messages to make space
        for (int i = 0; i < DEBUG_MAX_LOGS_HISTORY - 1; i++) {
            DebugLogs[i] = DebugLogs[i + 1];
        }
        DebugLogsIndex = DEBUG_MAX_LOGS_HISTORY - 1;
    }

    DebugLogs[DebugLogsIndex].type = msgType;
    DebugLogs[DebugLogsIndex].text = logMessage;
    DebugLogsIndex++;

    printf("%s\n", logMessage);
}

void LogDebug(const char* text, ...)
{
    va_list args;
    va_start(args, text);
    CustomLogger(LOG_DEBUG, text, args);
    va_end(args);
}

void Log(int msgType, const char* text, ...)
{
    va_list args;
    va_start(args, text);
    CustomLogger(msgType, text, args);
    va_end(args);
}

Color GetTextColorFromLogType(TraceLogLevel level)
{
	switch (level)
	{
    case LOG_DEBUG: return DARKGRAY;
	case LOG_INFO: return WHITE;
	case LOG_WARNING: return YELLOW;
	case LOG_ERROR: return RED;
    case LOG_FATAL: return ORANGE;
	default: return WHITE;
	}
}

void DrawDragableItemFrame(Ingredient i) {
    DrawTextureRec(i.texture, i.frameRectangle, i.position, RAYWHITE);
    if (options->showDebug && debugToolToggles.showObjects)
    {
        DrawRectangleLinesEx((Rectangle) { i.position.x, i.position.y, i.frameRectangle.width, i.frameRectangle.height }, 1, RED);
        DrawRectangle(i.position.x, i.position.y - 20, 300, 20, Fade(GRAY, 0.7));
        DrawTextEx(meowFont, TextFormat("%s | XY %.2f,%.2f", "Ingredient", i.position.x, i.position.y), (Vector2) { i.position.x, i.position.y - 20 }, 20, 1, WHITE);
    }
}


void DrawMenuFallingItems(double deltaTime, bool behide)
{
    int startIndex = behide ? 0 : 11;
    int endIndex = behide ? 11 : 20;

    for (int i = startIndex; i < endIndex; i++) {
        MenuFallingItem* item = &menuFallingItems[i];

        item->position.y += item->fallingSpeed * deltaTime;
        item->rotation += item->rotationSpeed * deltaTime;

        Vector2 origin = { (float)menuFallingItemTextures[item->textureIndex].width / 2, (float)menuFallingItemTextures[item->textureIndex].height / 2 };
        DrawTexturePro(menuFallingItemTextures[item->textureIndex], (Rectangle) { 0, 0, menuFallingItemTextures[item->textureIndex].width, menuFallingItemTextures[item->textureIndex].height },
            (Rectangle) {
            item->position.x, item->position.y, menuFallingItemTextures[item->textureIndex].width, menuFallingItemTextures[item->textureIndex].height
        },
            origin, item->rotation, WHITE);

        if (options->showDebug && debugToolToggles.showObjects) {
            Vector2 corners[4];
            corners[0] = (Vector2){ -origin.x, -origin.y };
            corners[1] = (Vector2){ -origin.x, origin.y };
            corners[2] = (Vector2){ origin.x, origin.y };
            corners[3] = (Vector2){ origin.x, -origin.y };

            for (int j = 0; j < 4; j++) {
                float tempX = corners[j].x * cos(DEG2RAD * item->rotation) - corners[j].y * sin(DEG2RAD * item->rotation);
                float tempY = corners[j].x * sin(DEG2RAD * item->rotation) + corners[j].y * cos(DEG2RAD * item->rotation);
                corners[j].x = tempX + item->position.x;
                corners[j].y = tempY + item->position.y;
            }

            DrawLineEx(corners[0], corners[1], 1, RED);
            DrawLineEx(corners[1], corners[2], 1, RED);
            DrawLineEx(corners[2], corners[3], 1, RED);
            DrawLineEx(corners[3], corners[0], 1, RED);

            DrawRectangle(item->position.x, item->position.y, 550, 20, Fade(GRAY, 0.7));
            DrawTextEx(meowFont, TextFormat("%d | XY %.2f,%.2f | R %.2f | G %.2f | Behide %s", i, item->position.x, item->position.y, item->rotation, item->fallingSpeed, behide ? "[Yes]" : "[No]"), (Vector2) { item->position.x, item->position.y }, 20, 1, WHITE);
        }

        if (item->position.y > baseY + BASE_SCREEN_HEIGHT + 1000) {
            item->position = (Vector2){ GetRandomDoubleValue(baseX, baseX + BASE_SCREEN_WIDTH - 20), baseY - GetRandomDoubleValue(200, 1000) };
            item->textureIndex = GetRandomValue(0, menuFallingItemsNumber - 1);
            item->fallingSpeed = GetRandomDoubleValue(1, 3);
            item->fallingSpeed *= 100;
            item->rotation = GetRandomDoubleValue(-360, 360);
            item->rotationSpeed = GetRandomValue(-3, 3);
            item->rotationSpeed *= 100;
            if (fabsf(item->rotationSpeed) > item->fallingSpeed)
                item->rotationSpeed = item->fallingSpeed;
            if (item->rotationSpeed == 0)
                item->rotationSpeed = 1 * 100;
        }
    }
}

bool IsNight()
{
    return currentColorIndex == 3 && colorTransitionTime < 0.4;
}

void DrawMovingCloudAndStar(double deltaTime)
{
    int cloudCount = sizeof(movingClouds) / sizeof(movingClouds[0]);
    int starCount = sizeof(movingStars) / sizeof(movingStars[0]);

    if (IsNight())
    {

        for (int i = 0; i < starCount; i++) {

            MovingStar* cloud = &movingStars[i];

            bool fromRight = false;
            bool isInView = cloud->position.x >= baseX && cloud->position.x <= baseX + BASE_SCREEN_WIDTH && cloud->position.y >= baseY && cloud->position.y <= baseY + BASE_SCREEN_HEIGHT;

            // Calculate the X position based on time and direction
            if (fromRight) {
                cloud->position.x -= cloud->speed * deltaTime;
                // Check if the cloud has moved off the screen
                if ((float)cloud->position.x + ((float)(cloud->texture.width) * (float)(cloud->scale)) <= baseX) {
                    cloud->position.x = baseX + BASE_SCREEN_WIDTH + (cloud->texture.width * cloud->scale) + GetRandomDoubleValue(100, 500);
                }
            }
            else {
                cloud->position.x += cloud->speed * deltaTime;
                // Check if the cloud has moved off the screen
                if (cloud->position.x > baseX + BASE_SCREEN_WIDTH) {
                    cloud->position.x = baseX - (cloud->texture.width * cloud->scale) - GetRandomDoubleValue(100, 500);
                }
            }

            // Draw the cloud
            DrawTexture(cloud->texture, cloud->position.x, cloud->position.y, WHITE);

            // Debug
            if (options->showDebug && debugToolToggles.showObjects)
            {
                DrawRectangleLinesEx((Rectangle) { cloud->position.x, cloud->position.y, cloud->texture.width* cloud->scale, cloud->texture.height* cloud->scale }, 1, RED);
                DrawRectangle(cloud->position.x, cloud->position.y - 20, 300, 20, Fade(GRAY, 0.7));
                DrawTextEx(meowFont, TextFormat("%s | XY %.2f,%.2f | Speed %.2f | Scale %.2f", "Stars", cloud->position.x, cloud->position.y, cloud->speed, cloud->scale), (Vector2) { cloud->position.x, cloud->position.y - 20 }, 20, 1, WHITE);
            }
        }
    }
    else
    {
        for (int i = 0; i < cloudCount; i++) {
            MovingCloud* cloud = &movingClouds[i];

            bool fromRight = cloud->fromRight;
            bool isInView = cloud->position.x >= baseX && cloud->position.x <= baseX + BASE_SCREEN_WIDTH && cloud->position.y >= baseY && cloud->position.y <= baseY + BASE_SCREEN_HEIGHT;

            // Calculate the X position based on time and direction
            if (fromRight) {
                cloud->position.x -= cloud->speed * deltaTime;
                // Check if the cloud has moved off the screen
                if ((float)cloud->position.x + ((float)(cloud->texture.width) * (float)(cloud->scale)) <= baseX) {
                    cloud->position.x = baseX + BASE_SCREEN_WIDTH + (cloud->texture.width * cloud->scale) + GetRandomDoubleValue(100, 500);
                    cloud->position.y = GetRandomDoubleValue(baseY, 0);
                }
            }
            else {
                cloud->position.x += cloud->speed * deltaTime;
                // Check if the cloud has moved off the screen
                if (cloud->position.x > baseX + BASE_SCREEN_WIDTH) {
                    cloud->position.x = baseX - (cloud->texture.width * cloud->scale) - GetRandomDoubleValue(100, 500);
                    cloud->position.y = GetRandomDoubleValue(baseY, 0);
                }
            }

            // Draw the cloud
            DrawTexture(cloud->texture, cloud->position.x, cloud->position.y, WHITE);

            // Debug
            if (options->showDebug && debugToolToggles.showObjects)
            {
                DrawRectangleLinesEx((Rectangle) { cloud->position.x, cloud->position.y, cloud->texture.width* cloud->scale, cloud->texture.height* cloud->scale }, 1, RED);
                DrawRectangle(cloud->position.x, cloud->position.y - 20, 300, 20, Fade(GRAY, 0.7));
                DrawTextEx(meowFont, TextFormat("%s | XY %.2f,%.2f | Speed %.2f | Scale %.2f", "Cloud", cloud->position.x, cloud->position.y, cloud->speed, cloud->scale), (Vector2) { cloud->position.x, cloud->position.y - 20 }, 20, 1, WHITE);
            }
        }

    }
   
}

double RandomCustomerTimeoutBasedOnDifficulty()
{
    switch (options->difficulty)
    {
    case FREEPLAY_EASY:
    case EASY:
        return GetRandomDoubleValue(80, 150);
        break;
    case FREEPLAY_MEDIUM:
    case MEDIUM:
        return GetRandomDoubleValue(40, 60);
        break;
    case FREEPLAY_HARD:
    case HARD:
        return GetRandomDoubleValue(20, 40);
        break;
    default:
        return GetRandomDoubleValue(80, 150);
        break;
    }
}

void RandomCustomerInitialResetBasedOnDifficulty(double *values) {

    switch (options->difficulty)
	{
        case FREEPLAY_EASY:
        case EASY:
        	values[0] = GetRandomDoubleValue(4, 6);
        	values[1] = GetRandomDoubleValue(5, 15);
        	values[2] = GetRandomDoubleValue(10, 25);
        	break;
        case FREEPLAY_MEDIUM:
        case MEDIUM:
        	values[0] = GetRandomDoubleValue(2, 6);
        	values[1] = GetRandomDoubleValue(4, 10);
        	values[2] = GetRandomDoubleValue(12, 20);
        	break;
        case FREEPLAY_HARD:
        case HARD:
        	values[0] = GetRandomDoubleValue(2, 4);
        	values[1] = GetRandomDoubleValue(3, 6);
        	values[2] = GetRandomDoubleValue(6, 10);
        	break;
        default:
            values[0] = GetRandomDoubleValue(4, 6);
            values[1] = GetRandomDoubleValue(5, 15);
            values[2] = GetRandomDoubleValue(10, 25);
        	break;
    }


}


double RandomCustomerResetBasedOnDifficulty()
{
    switch (options->difficulty)
    {
    case FREEPLAY_EASY:
    case EASY:
        return GetRandomDoubleValue(30, 50);
        break;
    case FREEPLAY_MEDIUM:
    case MEDIUM:
        return GetRandomDoubleValue(15, 30);
        break;
    case FREEPLAY_HARD:
    case HARD:
        return GetRandomDoubleValue(1, 10);
        break;
    default:
        return GetRandomDoubleValue(30, 50);
        break;
    }
}

void RandomGenerateOrder(char *order)
{
    int random = GetRandomValue(0, 2);
    order[0] = '\0'; // Initialize the string

    //base case, either CP or GP
    if (GetRandomValue(0, 1))
        strcat(order, "CP");
    else
        strcat(order, "GP");

    strcat(order, "Y");

    // Very very small chance just to order tea without any creamer lol
    if (GetRandomValue(0, 100) == 0)
		return;

    // another base case, either CM or MI
    if (GetRandomValue(0, 1))
        strcat(order, "CM");
    else
        strcat(order, "MI");

    bool hasTopping = false;
    if (random >= 1)
    {
        if (GetRandomValue(0, 1))
        {
            hasTopping = true;
            if (GetRandomValue(0, 1))
                strcat(order, "MA");
            else
                strcat(order, "WC");
        }
    }
    if (hasTopping && random >= 2)
    {
        if (GetRandomValue(0, 1))
        {
            if (GetRandomValue(0, 1))
                strcat(order, "CA");
            else
                strcat(order, "CH");
        }
    }
    // Log the order
    LogDebug(TextFormat("New order: %s", order));
}


void DrawCustomer(Customer* customer)
{
    Vector2 pos = customer->position;
    int frame = customer->textureType;

    if (customer == NULL) return;

    if (customer->visible)
    {

        switch (customer->emotion)
        {
        case EMOTION_HAPPY:
            DrawTextureEx(!customer->eyesClosed ? customersImageData[frame].happy : customersImageData[frame].happyEyesClosed, pos, 0.0f, 1.0f / 2.0f, WHITE);
            break;
        case EMOTION_FRUSTRATED:
            DrawTextureEx(!customer->eyesClosed ? customersImageData[frame].frustrated : customersImageData[frame].frustratedEyesClosed, pos, 0.0f, 1.0f / 2.0f, WHITE);
            break;
        case EMOTION_ANGRY:
            DrawTextureEx(!customer->eyesClosed ? customersImageData[frame].angry : customersImageData[frame].angryEyesClosed, pos, 0.0f, 1.0f / 2.0f, WHITE);
            break;
        default:
            break;
        }

    }

    if (customer->visible && !customer->isDummy)
    {
        DrawTextureEx(bubbles, (Vector2) { pos.x + 350, pos.y + 100 }, 0.0f, 1.0f / 2.0f, WHITE);

        if (strstr(customer->order, "CPY") != NULL)
            DrawTextureEx(cocoaChon.texture, (Vector2) { pos.x + 375, pos.y + 100 }, 0.0f, 1.0f / 2.0f, WHITE);
        else if (strstr(customer->order, "GPY") != NULL)
            DrawTextureEx(greenChon.texture, (Vector2) { pos.x + 375, pos.y + 100 }, 0.0f, 1.0f / 2.0f, WHITE);

        if (strstr(customer->order, "CM") != NULL)
            // DrawTextureEx(condensedMilk.texture, (Vector2) {pos.x + 425, pos.y + 100}, 0.0f, 1.0f / 2.0f, WHITE);
            DrawTextureRec(condensedMilk.texture, condensedMilk.frameRectangle, (Vector2) { pos.x + 425, pos.y + 100 }, RAYWHITE);
        else if (strstr(customer->order, "MI") != NULL)
            // DrawTextureEx(normalMilk.texture, (Vector2) {pos.x + 425, pos.y + 100}, 0.0f, 1.0f / 2.0f, WHITE);
            DrawTextureRec(normalMilk.texture, normalMilk.frameRectangle, (Vector2) { pos.x + 425, pos.y + 100 }, RAYWHITE);

        if (strstr(customer->order, "MA") != NULL)
            // DrawTextureEx(marshMellow.texture, (Vector2) {pos.x + 375, pos.y + 150}, 0.0f, 1.0f / 2.0f, WHITE);
            DrawTextureRec(marshMellow.texture, marshMellow.frameRectangle, (Vector2) { pos.x + 375, pos.y + 150 }, RAYWHITE);
        else if (strstr(customer->order, "WC") != NULL)
            // DrawTextureEx(whippedCream.texture, (Vector2) {pos.x + 375, pos.y + 150}, 0.0f, 1.0f / 2.0f, WHITE);
            DrawTextureRec(whippedCream.texture, whippedCream.frameRectangle, (Vector2) { pos.x + 375, pos.y + 150 }, RAYWHITE);

        if (strstr(customer->order, "CA") != NULL)
            // DrawTextureEx(caramelSauce.texture, (Vector2) {pos.x + 425, pos.y + 150}, 0.0f, 1.0f / 2.0f, WHITE);
            DrawTextureRec(caramelSauce.texture, caramelSauce.frameRectangle, (Vector2) { pos.x + 425, pos.y + 150 }, RAYWHITE);
        else if (strstr(customer->order, "CH") != NULL)
            // DrawTextureEx(chocolateSauce.texture, (Vector2) {pos.x + 425, pos.y + 150}, 0.0f, 1.0f / 2.0f, WHITE);
            DrawTextureRec(chocolateSauce.texture, chocolateSauce.frameRectangle, (Vector2) { pos.x + 425, pos.y + 150 }, RAYWHITE);
    }

    if (options->showDebug && debugToolToggles.showObjects)
    {
        DrawRectangleLinesEx((Rectangle) { pos.x, pos.y, customersImageData[frame].happy.width / 2, customersImageData[frame].happy.height / 2 }, 1, RED);
        DrawRectangle(pos.x, pos.y - 20, 500, 60, Fade(GRAY, 0.7));
        DrawTextEx(meowFont, TextFormat("%s | Blink %s (%.2f) %.2f/%.2f", StringFromCustomerEmotionEnum(customer->emotion), customer->eyesClosed ? "[Yes]" : "[No]", customer->blinkDuration, customer->blinkTimer, customer->normalDuration), (Vector2) { pos.x, pos.y - 20 }, 20, 1, WHITE);
        if (customer->visible)
            DrawTextEx(meowFont, TextFormat("Timeout %.2f/%.2f", (float)customer->currentTime, (float)customer->orderEnd), (Vector2) { pos.x, pos.y }, 20, 1, WHITE);
        else
            DrawTextEx(meowFont, TextFormat("Reset %.2f/%.2f", (float)customer->currentTime, (float)customer->resetTimer), (Vector2) { pos.x, pos.y }, 20, 1, WHITE);
        DrawTextEx(meowFont, TextFormat("Visible %s | Order %s", customer->visible ? "[Yes]" : "[No]", customer->order), (Vector2) { pos.x, pos.y + 20 }, 20, 1, WHITE);
    }

}

void UpdateMenuCustomerBlink(Customer* customer, double deltaTime) {
    customer->blinkTimer += deltaTime;

    if (!customer->eyesClosed && customer->blinkTimer > customer->normalDuration) {
        customer->blinkTimer = 0.0;
        customer->eyesClosed = true;
    }
    else if (customer->eyesClosed && customer->blinkTimer > customer->blinkDuration) {
        customer->blinkTimer = 0.0;
        customer->eyesClosed = false;
    }
}

void DrawCustomerInMenu(double deltaTime) {
    UpdateMenuCustomerBlink(&menuCustomer1, deltaTime);
    UpdateMenuCustomerBlink(&menuCustomer2, deltaTime);

    // Update customer emotions according to difficulty
    switch (options->difficulty)
    {
        case FREEPLAY_EASY:
        case EASY:
            menuCustomer1.emotion = EMOTION_HAPPY;
            menuCustomer2.emotion = EMOTION_HAPPY;
            break;
        case FREEPLAY_MEDIUM:
        case MEDIUM:
            menuCustomer1.emotion = EMOTION_FRUSTRATED;
            menuCustomer2.emotion = EMOTION_FRUSTRATED;
            break;
        case FREEPLAY_HARD:
        case HARD:
            menuCustomer1.emotion = EMOTION_ANGRY;
            menuCustomer2.emotion = EMOTION_ANGRY;
            break;
            break;
        default:
            menuCustomer1.emotion = EMOTION_HAPPY;
            menuCustomer2.emotion = EMOTION_HAPPY;
            break;
    }
    DrawCustomer(&menuCustomer1);
    DrawCustomer(&menuCustomer2);
}

void DrawOuterWorld()
{
    // Draw area outside the view
    DrawRectangle(baseX, baseY - 2000, BASE_SCREEN_WIDTH, 2000, BLACK);
    DrawRectangle(baseX, baseY + BASE_SCREEN_HEIGHT, BASE_SCREEN_WIDTH, 2000, BLACK);
    DrawRectangle(baseX - 2000, baseY, 2000, BASE_SCREEN_HEIGHT, BLACK);
    DrawRectangle(baseX + BASE_SCREEN_WIDTH, baseY, 2000, BASE_SCREEN_HEIGHT, BLACK);
}

void SetRuntimeResolution(Camera2D *camera, int screenWidth, int screenHeight)
{
    SetWindowSize(screenWidth, screenHeight);
    float currentAspectRatio = (float)screenWidth / (float)screenHeight;
    float scale = (currentAspectRatio > targetAspectRatio) ? (float)screenHeight / BASE_SCREEN_HEIGHT : (float)screenWidth / BASE_SCREEN_WIDTH;

    camera->zoom = scale;
    camera->offset.x = screenWidth / 2.0f;
    camera->offset.y = screenHeight / 2.0f;

    options->resolution.x = screenWidth;
    options->resolution.y = screenHeight;
}

bool IsMousePositionInGameWindow(Camera2D * camera)
{
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), *camera);
    return mouseWorldPos.x >= baseX && mouseWorldPos.x <= baseX + BASE_SCREEN_WIDTH && mouseWorldPos.y >= baseY && mouseWorldPos.y <= baseY + BASE_SCREEN_HEIGHT;

}

void DrawDebugLogs(Camera2D* camera)
{
    DrawRectangle(baseX, baseY + BASE_SCREEN_HEIGHT - 20 - (DEBUG_MAX_LOGS_HISTORY * 20), BASE_SCREEN_WIDTH, DEBUG_MAX_LOGS_HISTORY * 20 + 20, Fade(GRAY, 0.7));

    for (int i = 0; i < DEBUG_MAX_LOGS_HISTORY; i++) {
        int index = DEBUG_MAX_LOGS_HISTORY - i - 1;
		if (DebugLogs[index].text != NULL) {
			DrawTextEx(meowFont, DebugLogs[index].text, (Vector2) { baseX + 10, baseY + BASE_SCREEN_HEIGHT - 20 - (i * 20) }, 16, 1, GetTextColorFromLogType(DebugLogs[index].type));
		}
        else {
			break;
		}
	}
}

void DrawFpsGraph(Camera2D* camera)
{
    int graphWidth = DEBUG_MAX_FPS_HISTORY;
    int graphHeight = 200;
    int graphX = baseX + BASE_SCREEN_WIDTH - 15 - graphWidth;
    int graphY = baseY + 25;

    // Calculate the maximum value in the FPS history
    float maxFpsValue = 0.0f;
    for (int i = 0; i < DEBUG_MAX_FPS_HISTORY; i++) {
        if (DebugFpsHistory[i] > maxFpsValue) {
            maxFpsValue = (float)DebugFpsHistory[i];
        }
    }

    // Adjust the scale based on the maximum value or target FPS
    float fpsScale;
    if ((float)options->targetFps > maxFpsValue) {
        fpsScale = (float)graphHeight / (float)options->targetFps;
    }
    else {
        fpsScale = (float)graphHeight / maxFpsValue;
    }

    DrawRectangle(graphX, graphY, graphWidth, graphHeight, Fade(GRAY, 0.7));

    // Draw the FPS history graph with color based on target FPS
    for (int i = 1; i < DEBUG_MAX_FPS_HISTORY; i++) {
        int x1 = graphX + i - 1;
        int x2 = graphX + i;
        int y1 = graphY + graphHeight - (DebugFpsHistory[(DebugFpsHistoryIndex + i - 1) % DEBUG_MAX_FPS_HISTORY] * fpsScale);
        int y2 = graphY + graphHeight - (DebugFpsHistory[(DebugFpsHistoryIndex + i) % DEBUG_MAX_FPS_HISTORY] * fpsScale);

        // Calculate the ratio of actual FPS to target FPS
        float fpsRatio = (float)DebugFpsHistory[(DebugFpsHistoryIndex + i) % DEBUG_MAX_FPS_HISTORY] / (float)options->targetFps;

        // Set color based on the ratio to target FPS
        Color lineColor;
        if (fpsRatio >= 0.9f) {
            lineColor = GREEN; // FPS close to or exceeding target
        }
        else if (fpsRatio >= 0.7f) {
            lineColor = YELLOW; // FPS above 70% of target
        }
        else {
            lineColor = RED; // FPS below 70% of target
        }

        DrawLine(x1, y1, x2, y2, lineColor);
    }

    DrawTextEx(meowFont, "FPS", (Vector2) { baseX + BASE_SCREEN_WIDTH - 50, baseY + 5 }, 20, 2, GRAY);
    DrawTextEx(meowFont, TextFormat("%.2f", maxFpsValue > options->targetFps ? maxFpsValue : options->targetFps), (Vector2) { graphX + 10, graphY + 10 }, 15, 2, WHITE);
    DrawTextEx(meowFont, "0", (Vector2) { graphX + 10, graphY + graphHeight - 30 }, 15, 2, WHITE);
}


void DrawFrameTime(Camera2D* camera) {
    int graphWidth = DEBUG_MAX_FPS_HISTORY;
    int graphHeight = 200;
    int graphX = baseX + BASE_SCREEN_WIDTH - 15 - graphWidth;
    int graphY = baseY + 255;

    float targetFrameTime = 1000.0f / options->targetFps;

    // Find the maximum value in the DebugFrameTimeHistory array
    float maxFrameTime = 0.0f;
    for (int i = 0; i < DEBUG_MAX_FPS_HISTORY; i++) {
        if (DebugFrameTimeHistory[i] > maxFrameTime) {
            maxFrameTime = DebugFrameTimeHistory[i];
        }
    }

    // Calculate the expected value based on the target FPS
    float expectedFrameTime = 1000.0f / options->targetFps;

    // Adjust the frameTimeScale based on the maximum value
    float frameTimeScale = (maxFrameTime > 0) ? (float)graphHeight / maxFrameTime : 1.0f;

    DrawRectangle(graphX, graphY, graphWidth, graphHeight, Fade(GRAY, 0.7));

    // Draw the frame time history graph with color based on expected value
    for (int i = 1; i < DEBUG_MAX_FPS_HISTORY; i++) {
        int x1 = graphX + i - 1;
        int x2 = graphX + i;
        int y1 = graphY + graphHeight - (DebugFrameTimeHistory[(DebugFrameTimeHistoryIndex + i - 1) % DEBUG_MAX_FPS_HISTORY] * frameTimeScale);
        int y2 = graphY + graphHeight - (DebugFrameTimeHistory[(DebugFrameTimeHistoryIndex + i) % DEBUG_MAX_FPS_HISTORY] * frameTimeScale);

        // Calculate the ratio of the frame time to the expected frame time
        float ratioToExpected = DebugFrameTimeHistory[(DebugFrameTimeHistoryIndex + i) % DEBUG_MAX_FPS_HISTORY] / expectedFrameTime;

        // Set color based on the ratio to expected value
        Color lineColor;
        if (ratioToExpected <= 1.0f) {
            lineColor = GREEN;
        }
        else if (ratioToExpected <= 1.5f) {
            lineColor = YELLOW;
        }
        else {
            lineColor = RED;
        }

        DrawLine(x1, y1, x2, y2, lineColor);
    }

    DrawTextEx(meowFont, "Frame Time (ms)", (Vector2) { baseX - 135 + BASE_SCREEN_WIDTH - 50, graphY - 25 }, 20, 2, GRAY);
    DrawTextEx(meowFont, TextFormat("%.2f", maxFrameTime > expectedFrameTime ? maxFrameTime : expectedFrameTime), (Vector2) { graphX + 10, graphY + 10 }, 15, 2, WHITE);
    DrawTextEx(meowFont, "0", (Vector2) { graphX + 10, graphY + graphHeight - 30 }, 15, 2, WHITE);
}


void UpdateDebugFpsHistory() {
    DebugFpsHistory[DebugFpsHistoryIndex] = GetFPS();
    DebugFpsHistoryIndex = (DebugFpsHistoryIndex + 1) % DEBUG_MAX_FPS_HISTORY;
}

void UpdateDebugFrameTimeHistory() {
	DebugFrameTimeHistory[DebugFrameTimeHistoryIndex] = GetFrameTime() * 1000;
	DebugFrameTimeHistoryIndex = (DebugFrameTimeHistoryIndex + 1) % DEBUG_MAX_FPS_HISTORY;
}

void DrawDebugStats(Camera2D* camera)
{
    DrawRectangle(baseX, baseY, 1100, 70, Fade(GRAY, 0.7));

    Color color = GREEN;
    int fps = GetFPS();

    if (fps < options->targetFps * 0.7f) {
		color = RED;
	}
	else if (fps < options->targetFps * 0.9f) {
		color = YELLOW;
	}

    Vector2 mousePosition = GetMousePosition();
    Vector2 mouseWorldPos = GetScreenToWorld2D(mousePosition, *camera);

    DrawTextEx(meowFont, TextFormat("%d FPS | Target FPS %d | Window (%dx%d) | Render (%dx%d) | Fullscreen ", fps, options->targetFps, options->resolution.x, options->resolution.y, BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT, options->fullscreen ? "[Yes]" : "[No]"), (Vector2) { baseX + 10, baseY + 5 }, 20, 2, color);
    DrawTextEx(meowFont, TextFormat("Cursor %.2f,%.2f (%dx%d) | World %.2f,%.2f (%dx%d) | R Base World %.2f,%.2f", mousePosition.x, mousePosition.y, options->resolution.x, options->resolution.y, mouseWorldPos.x, mouseWorldPos.y, BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT, mouseWorldPos.x - baseX, mouseWorldPos.y - baseY), (Vector2) { baseX + 10, baseY + 25 }, 20, 2, WHITE);
    DrawTextEx(meowFont, TextFormat("Zoom %.2f | In View %s", camera->zoom, IsMousePositionInGameWindow(camera) ? "[Yes]" : "[No]"), (Vector2) { baseX + 10, baseY + 45 }, 20, 2, WHITE);
}

void DrawDebugOverlay(Camera2D *camera)
{
    if (options->showDebug)
    {
        // F1 - Toggle debug logs
        if (IsKeyPressed(KEY_F1))
        {
            debugToolToggles.showDebugLogs = !debugToolToggles.showDebugLogs;
        }
        // F2 - Toggle debug stats
        else if (IsKeyPressed(KEY_F2))
        {
            debugToolToggles.showStats = !debugToolToggles.showStats;
        }
        // F3 - Toggle debug graph
        else if (IsKeyPressed(KEY_F3))
        {
            debugToolToggles.showGraph = !debugToolToggles.showGraph;
        }
        //  F4 - Toggle debug objects
        else if (IsKeyPressed(KEY_F4))
        {
            debugToolToggles.showObjects = !debugToolToggles.showObjects;
        }
    }

    UpdateDebugFpsHistory();
    UpdateDebugFrameTimeHistory();

    DrawRectangle(baseX + BASE_SCREEN_WIDTH - 15 - 300, baseY + BASE_SCREEN_HEIGHT - 15 - 110, 300, 140, Fade(GRAY, 0.7));
    DrawTextEx(meowFont, "Debug Tools", (Vector2) { baseX + BASE_SCREEN_WIDTH - 15 - 290, baseY + BASE_SCREEN_HEIGHT - 15 - 100 }, 20, 2, WHITE);
    DrawTextEx(meowFont, TextFormat("Logs | %s | F1", debugToolToggles.showDebugLogs ? "[On]" : "[Off]"), (Vector2) { baseX + BASE_SCREEN_WIDTH - 15 - 290, baseY + BASE_SCREEN_HEIGHT - 15 - 80 }, 20, 2, debugToolToggles.showDebugLogs ? GREEN : WHITE);
    DrawTextEx(meowFont, TextFormat("Stats | %s | F2", debugToolToggles.showStats ? "[On]" : "[Off]"), (Vector2) { baseX + BASE_SCREEN_WIDTH - 15 - 290, baseY + BASE_SCREEN_HEIGHT - 15 - 60 }, 20, 2, debugToolToggles.showStats ? GREEN : WHITE);
    DrawTextEx(meowFont, TextFormat("Graph | %s | F3", debugToolToggles.showGraph ? "[On]" : "[Off]"), (Vector2) { baseX + BASE_SCREEN_WIDTH - 15 - 290, baseY + BASE_SCREEN_HEIGHT - 15 - 40 }, 20, 2, debugToolToggles.showGraph ? GREEN : WHITE);
    DrawTextEx(meowFont, TextFormat("Objects | %s | F4", debugToolToggles.showObjects ? "[On]" : "[Off]"), (Vector2) { baseX + BASE_SCREEN_WIDTH - 15 - 290, baseY + BASE_SCREEN_HEIGHT - 15 - 20 }, 20, 2, debugToolToggles.showObjects ? GREEN : WHITE);
    
    if (debugToolToggles.showDebugLogs)
        DrawDebugLogs(camera);
    if(debugToolToggles.showStats)
        DrawDebugStats(camera);
    if (debugToolToggles.showGraph)
    {
        DrawFpsGraph(camera);
        DrawFrameTime(camera);
    }

}

Customer CreateCustomerWithOrder(int patience, double currentTime, int orderEnd, Vector2 pos, int textureType, double resetTimer) {
    Customer newCustomer = CreateCustomer(EMOTION_HAPPY, 0, 0, 0.25, true, pos, textureType, resetTimer);
    RandomCustomerBlinkTime(&newCustomer);

    newCustomer.currentTime = currentTime;
    newCustomer.orderEnd = orderEnd * patience;
	strcpy(newCustomer.order, "");
	RandomGenerateOrder(newCustomer.order);
    return newCustomer;
}

// void create_order(Order *order, char *first, char *second, char *third, char *fourth) {
// 	order->first = first;
// 	order->second = second;
// 	order->third = third;
// 	order->fourth = fourth;
// }

// void distribute_points(Order* order, char* first, char* second, char* third, char* fourth)
// {
//     if (first)
//     {
//         global_score += 50;
//     }
//     if (second)
//     {
//         global_score += 50;
//     }
//     if (third)
//     {
//         global_score += 50;
//     }
//     if (fourth)
//     {
//         global_score += 50;
//     }
// }

bool validiator(Customer *customer, char *order)
{
    LogDebug("Validating order: %s against %s", order, customer->order);
	if (strcmp(customer->order, order) == 0)
	{
        return true;
	}
	else
	{
        return false;
	}
}

//create customer image at either position 1 2 or 3
void render_customers(Customers *customers)
{
    if(customers != NULL)  // Check the customers pointer instead of address
    {
        DrawCustomer(&customers->customer1);
        DrawCustomer(&customers->customer2);
        DrawCustomer(&customers->customer3);
    }
}

//Yandere dev inspired programming.

void RemoveCustomer(Customer *customer)
{
	customer->visible = false;
    customer->currentTime = 0;
    customer->orderEnd = 0;
    customer->resetTimer = RandomCustomerResetBasedOnDifficulty();
}

void UpdateCustomerState(Customer* customer, float deltaTime) {

    customer->currentTime += deltaTime;

    if (customer->visible == true) {

        if ((float)customer->currentTime < (float)customer->orderEnd) {
            // Calculate a ratio of how close to the orderEnd the currentTime is
            float ratio = (float)customer->currentTime / (float)customer->orderEnd;

            if (ratio > 0.75) {
                customer->emotion = EMOTION_ANGRY;
            }
            else if (ratio > 0.5) {
                customer->emotion = EMOTION_FRUSTRATED;
            }
            else {
                customer->emotion = EMOTION_HAPPY;
            }
        }
        else {
            PlaySoundFx(FX_ANGRY);
            RemoveCustomer(customer);
            global_score -= 50;
        }
    }
    else {

        if ((float)customer->currentTime > customer->resetTimer) {
            customer->currentTime = 0;
            customer->orderEnd = RandomCustomerTimeoutBasedOnDifficulty();
            customer->visible = true;
            strcpy(customer->order, "");
            RandomGenerateOrder(customer->order);
        }
    }
}



void Tick(Customers *customers, float deltaTime) {
    UpdateCustomerState(&customers->customer1, deltaTime);
    UpdateCustomerState(&customers->customer2, deltaTime);
    UpdateCustomerState(&customers->customer3, deltaTime);
}

void ResetGameState();
void PlaySoundFx(SoundFxType type) {
    int randomIndex = 0;

    if (!options->soundFxEnabled) return;

    switch (type) {
    case FX_ANGRY: {
        Sound angrySounds[] = { angry1Fx, angry2Fx, angry3Fx, angry4Fx };
        randomIndex = rand() % (sizeof(angrySounds) / sizeof(angrySounds[0]));
        PlaySound(angrySounds[randomIndex]);
        break;
    }
    case FX_BOTTLE: {
        Sound bottleSounds[] = { bottle1Fx, bottle2Fx, bottle3Fx };
        randomIndex = rand() % (sizeof(bottleSounds) / sizeof(bottleSounds[0]));
        PlaySound(bottleSounds[randomIndex]);
        break;
    }
    case FX_CONFUSED: {
        Sound confusedSounds[] = { confused1Fx, confused2Fx, confused3Fx, confused4Fx };
        randomIndex = rand() % (sizeof(confusedSounds) / sizeof(confusedSounds[0]));
        PlaySound(confusedSounds[randomIndex]);
        break;
    }
    case FX_CORRECT:
        PlaySound(correctFx);
        break;
    case FX_DROP: {
        Sound dropSounds[] = { drop1Fx, drop2Fx, drop3Fx };
        randomIndex = rand() % (sizeof(dropSounds) / sizeof(dropSounds[0]));
        PlaySound(dropSounds[randomIndex]);
        break;
    }
    case FX_PICKUP: {
        Sound pickupSounds[] = { pickup1Fx, pickup2Fx, pickup3Fx };
        randomIndex = rand() % (sizeof(pickupSounds) / sizeof(pickupSounds[0]));
        PlaySound(pickupSounds[randomIndex]);
        break;
    }
    case FX_POUR: {
        Sound pourSounds[] = { pour1Fx, pour2Fx, pour3Fx };
        randomIndex = rand() % (sizeof(pourSounds) / sizeof(pourSounds[0]));
        PlaySound(pourSounds[randomIndex]);
        break;
    }
    case FX_STIR: {
        Sound stirSounds[] = { stir1Fx, stir2Fx, stir3Fx };
        randomIndex = rand() % (sizeof(stirSounds) / sizeof(stirSounds[0]));
        PlaySound(stirSounds[randomIndex]);
        break;
    }
    case FX_BOONG:
        PlaySound(boongFx);
        break;
    case FX_HOVER:
        PlaySound(hoverFx);
        break;
    case FX_SELECT:
        PlaySound(selectFx);
        break;
    case FX_FLICK:
        PlaySound(flickFx);
    }
}
void WindowUpdate(Camera2D* camera)
{
    if (IsWindowResized())
    {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();


        float currentAspectRatio = (float)screenWidth / (float)screenHeight;
        float scale = (currentAspectRatio > targetAspectRatio) ? (float)screenHeight / BASE_SCREEN_HEIGHT : (float)screenWidth / BASE_SCREEN_WIDTH;

        camera->zoom = scale;
        camera->offset.x = screenWidth / 2.0f;
        camera->offset.y = screenHeight / 2.0f;

        options->resolution.x = screenWidth;
        options->resolution.y = screenHeight;
    }

    // Alt - Enter fullscreen
    if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER) || IsKeyDown(KEY_RIGHT_ALT) && IsKeyPressed(KEY_ENTER))
    {
        SetRuntimeResolution(camera, BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT);
        ToggleFullscreen();
    }

    if(currentBgm != NULL)
        UpdateMusicStream(*currentBgm);
}

void LoadGlobalAssets()
{
    double startTime = GetTime();
    backgroundTexture = LoadTexture(ASSETS_PATH"image/backgrounds/main.png");
    backgroundOverlayTexture = LoadTexture(ASSETS_PATH"image/backgrounds/main_overlay_1.png");
    backgroundOverlaySidebarTexture = LoadTexture(ASSETS_PATH"image/backgrounds/main_overlay_2.png");
    pawTexture = LoadTexture(ASSETS_PATH"image/elements/paw.png");

    checkbox = LoadTexture(ASSETS_PATH"image/elements/checkbox.png");
    checkboxChecked = LoadTexture(ASSETS_PATH"image/elements/checkbox_checked.png");
    left_arrow = LoadTexture(ASSETS_PATH"image/elements/left_arrow.png");
    right_arrow = LoadTexture(ASSETS_PATH"image/elements/right_arrow.png");

	customerTexture_first_happy = LoadTexture(ASSETS_PATH"image/sprite/customer_happy.png");
	customerTexture_second_happy = LoadTexture(ASSETS_PATH"image/sprite/customer_happy.png");
	customerTexture_third_happy = LoadTexture(ASSETS_PATH"image/sprite/customer_happy.png");
	customerTexture_first_normal = LoadTexture(ASSETS_PATH"image/sprite/customer_normal.png");
	customerTexture_second_normal = LoadTexture(ASSETS_PATH"image/sprite/customer_normal.png");
	customerTexture_third_normal = LoadTexture(ASSETS_PATH"image/sprite/customer_normal.png");
	customerTexture_first_angry = LoadTexture(ASSETS_PATH"image/sprite/customer_angry.png");
	customerTexture_second_angry = LoadTexture(ASSETS_PATH"image/sprite/customer_angry.png");
	customerTexture_third_angry = LoadTexture(ASSETS_PATH"image/sprite/customer_angry.png");

    hoverFx = LoadSound(ASSETS_PATH"audio/hover.wav");
    selectFx = LoadSound(ASSETS_PATH"audio/select.wav");
    boongFx = LoadSound(ASSETS_PATH"audio/boong.wav");

    angry1Fx = LoadSound(ASSETS_PATH"audio/angry_1.wav");
    angry2Fx = LoadSound(ASSETS_PATH"audio/angry_2.wav");
    angry3Fx = LoadSound(ASSETS_PATH"audio/angry_3.wav");
    angry4Fx = LoadSound(ASSETS_PATH"audio/angry_4.wav");

    bottle1Fx = LoadSound(ASSETS_PATH"audio/bottle_1.wav");
    bottle2Fx = LoadSound(ASSETS_PATH"audio/bottle_2.wav");
    bottle3Fx = LoadSound(ASSETS_PATH"audio/bottle_3.wav");

    confused1Fx = LoadSound(ASSETS_PATH"audio/confused_1.wav");
    confused2Fx = LoadSound(ASSETS_PATH"audio/confused_2.wav");
    confused3Fx = LoadSound(ASSETS_PATH"audio/confused_3.wav");
    confused4Fx = LoadSound(ASSETS_PATH"audio/confused_4.wav");

    correctFx = LoadSound(ASSETS_PATH"audio/correct.wav");

    drop1Fx = LoadSound(ASSETS_PATH"audio/drop_1.wav");
    drop2Fx = LoadSound(ASSETS_PATH"audio/drop_2.wav");
    drop3Fx = LoadSound(ASSETS_PATH"audio/drop_3.wav");

    pickup1Fx = LoadSound(ASSETS_PATH"audio/pickup_1.wav");
    pickup2Fx = LoadSound(ASSETS_PATH"audio/pickup_2.wav");
    pickup3Fx = LoadSound(ASSETS_PATH"audio/pickup_3.wav");

    pour1Fx = LoadSound(ASSETS_PATH"audio/pour_1.wav");
    pour2Fx = LoadSound(ASSETS_PATH"audio/pour_2.wav");
    pour3Fx = LoadSound(ASSETS_PATH"audio/pour_3.wav");

    stir1Fx = LoadSound(ASSETS_PATH"audio/stir_1.wav");
    stir2Fx = LoadSound(ASSETS_PATH"audio/stir_2.wav");
    stir3Fx = LoadSound(ASSETS_PATH"audio/stir_3.wav");

    flickFx = LoadSound(ASSETS_PATH"audio/flick.wav");

    menuFallingItemTextures[0] = LoadTexture(ASSETS_PATH"image/falling_items/cara.png");
    menuFallingItemTextures[1] = LoadTexture(ASSETS_PATH"image/falling_items/cmilk.png");
    menuFallingItemTextures[2] = LoadTexture(ASSETS_PATH"image/falling_items/cocoa.png");
    menuFallingItemTextures[3] = LoadTexture(ASSETS_PATH"image/falling_items/gar.png");
    menuFallingItemTextures[4] = LoadTexture(ASSETS_PATH"image/falling_items/marshmello.png");
    menuFallingItemTextures[5] = LoadTexture(ASSETS_PATH"image/falling_items/matcha.png");
    menuFallingItemTextures[6] = LoadTexture(ASSETS_PATH"image/falling_items/milk.png");
    menuFallingItemTextures[7] = LoadTexture(ASSETS_PATH"image/falling_items/wcream.png");

    teaPowderTexture = LoadTexture(ASSETS_PATH"/spritesheets/GP.png");
    cocoaPowderTexture = LoadTexture(ASSETS_PATH"/spritesheets/CP.png");
    caramelSauceTexture = LoadTexture(ASSETS_PATH"/spritesheets/CA.png");
    chocolateSauceTexture = LoadTexture(ASSETS_PATH"/spritesheets/CH.png");
    condensedMilkTexture = LoadTexture(ASSETS_PATH"/spritesheets/CM.png");
    normalMilkTexture = LoadTexture(ASSETS_PATH"/spritesheets/MI.png");
    marshMellowTexture = LoadTexture(ASSETS_PATH"/spritesheets/MA.png");
    whippedCreamTexture = LoadTexture(ASSETS_PATH"/spritesheets/WC.png");
    hotWaterTexture = LoadTexture(ASSETS_PATH"/spritesheets/GAR.png");
    greenChonTexture = LoadTexture(ASSETS_PATH"/spritesheets/greenchon.png");
    cocoaChonTexture = LoadTexture(ASSETS_PATH"/spritesheets/cocoachon.png");

    trashCanTexture = LoadTexture(ASSETS_PATH"spritesheets/TRASHCAN.png");

    for (int i = 0; i < 3; i++)
    {
        customersImageData[i].happy = LoadTexture(TextFormat(ASSETS_PATH"image/sprite/customer_%d/happy.png", i + 1));
		customersImageData[i].happyEyesClosed = LoadTexture(TextFormat(ASSETS_PATH"image/sprite/customer_%d/happy_eyes_closed.png", i + 1));
        customersImageData[i].frustrated = LoadTexture(TextFormat(ASSETS_PATH"image/sprite/customer_%d/frustrated.png", i + 1));
        customersImageData[i].frustratedEyesClosed = LoadTexture(TextFormat(ASSETS_PATH"image/sprite/customer_%d/frustrated_eyes_closed.png", i + 1));
        customersImageData[i].angry = LoadTexture(TextFormat(ASSETS_PATH"image/sprite/customer_%d/angry.png", i + 1));
        customersImageData[i].angryEyesClosed = LoadTexture(TextFormat(ASSETS_PATH"image/sprite/customer_%d/angry_eyes_closed.png", i + 1));
	}

    cloud1Texture = LoadTexture(ASSETS_PATH"image/sprite/cloud_1.png");
    cloud2Texture = LoadTexture(ASSETS_PATH"image/sprite/cloud_2.png");
    cloud3Texture = LoadTexture(ASSETS_PATH"image/sprite/cloud_3.png");

    star1Texture = LoadTexture(ASSETS_PATH"image/sprite/star_1.png");
    star2Texture = LoadTexture(ASSETS_PATH"image/sprite/star_2.png");
	//orders
	bubbles = LoadTexture(ASSETS_PATH"image/elements/bubbles.png");

	//

    menuBgm = LoadMusicStream(ASSETS_PATH"audio/bgm/Yojo_Summer_My_Heart.wav");

    menuCustomer1 = CreateCustomer(EMOTION_HAPPY, 2.0, 4.0, 0.25, true, (Vector2) { baseX + 650, baseY + 55 }, 1, 0);
    menuCustomer2 = CreateCustomer(EMOTION_HAPPY, 0.4, 5.2, 0.3, true, (Vector2) { baseX + 1200, baseY + 52 }, 2, 0);
    RandomCustomerBlinkTime(&menuCustomer1);
    RandomCustomerBlinkTime(&menuCustomer2);
    menuCustomer1.isDummy = true;
    menuCustomer2.isDummy = true;

    loadDurationTimer = GetTime() - startTime;
    isGlobalAssetsLoadFinished = true;
}

void UnloadGlobalAssets()
{
    UnloadTexture(backgroundTexture);
    UnloadTexture(backgroundOverlayTexture);
    UnloadTexture(backgroundOverlaySidebarTexture);
    UnloadTexture(pawTexture);
    UnloadFont(meowFont);

    UnloadTexture(checkbox);
	UnloadTexture(checkboxChecked);
    UnloadTexture(left_arrow);
    UnloadTexture(right_arrow);

    UnloadSound(hoverFx);
    UnloadSound(selectFx);

    for(int i = 0; i < 8; i++)
		UnloadTexture(menuFallingItemTextures[i]);

    UnloadTexture(logoTexture);
    UnloadTexture(splashBackgroundTexture);
    UnloadTexture(splashOverlayTexture);

    for (int i = 0; i < 3; i++)
    {
        UnloadTexture(customersImageData[i].happy);
		UnloadTexture(customersImageData[i].happyEyesClosed);
		UnloadTexture(customersImageData[i].frustrated);
		UnloadTexture(customersImageData[i].frustratedEyesClosed);
		UnloadTexture(customersImageData[i].angry);
		UnloadTexture(customersImageData[i].angryEyesClosed);
    }

    UnloadMusicStream(menuBgm);
}


void PlayBgm(Music *bgm)
{
    if (bgm == currentBgm)
    {
        if (isCurrentBgmPaused && options->musicEnabled)
        {
            PlayMusicStream(*bgm);
            bgm->looping = true;
            isCurrentBgmPaused = false;
        }
        return;
    }

    currentBgm = bgm;
	StopMusicStream(*currentBgm);
	PlayMusicStream(*currentBgm);
    SetMusicVolume(*currentBgm, bgmVolume);
    bgm->looping = true;
    isCurrentBgmPaused = false;
}

void PlayBgmIfStopped(Music* bgm)
{
    if (bgm == currentBgm)
    {
        if (isCurrentBgmPaused && options->musicEnabled)
        {
            PlayMusicStream(*bgm);
            SetMusicVolume(*bgm, bgmVolume);
            bgm->looping = true;
            isCurrentBgmPaused = false;
        }
        return;
    }

    currentBgm = bgm;
    PlayMusicStream(*currentBgm);
    SetMusicVolume(*currentBgm, bgmVolume);
    bgm->looping = true;
    isCurrentBgmPaused = false;
}

void PauseBgm(Music *bgm)
{
	if (bgm != currentBgm) return;
	if (isCurrentBgmPaused) return;

	PauseMusicStream(*bgm);
	isCurrentBgmPaused = true;
}

void StopBgm(Music *bgm)
{
	StopMusicStream(*bgm);
    isCurrentBgmPaused = false;
    currentBgm = NULL;
}

Color ColorAlphaOverride(Color color, float alpha)
{
	return (Color) { color.r, color.g, color.b, (unsigned char)(alpha * 255) };
}

void DrawDayNightCycle(double deltaTime)
{
    const Color dayNightColors[] = {
        (Color){173, 216, 230, 255},  // Morning (Anime Light Blue)
        (Color){0, 102, 204, 255},    // Afternoon (Anime Blue)
        (Color){245, 161, 59, 255},    // Evening (Anime Orange)
        (Color){0, 0, 102, 255}       // Night (Anime Dark Blue)
    };

    float colorTransitionSpeed = (float)(sizeof(dayNightColors) / sizeof(dayNightColors[0])) / dayNightCycleDuration;

    // Determine the color to interpolate from and to
    int fromColorIndex = currentColorIndex;
    int toColorIndex = (currentColorIndex + 1) % (sizeof(dayNightColors) / sizeof(dayNightColors[0]));

    // Calculate the interpolation factor (0 to 1) based on colorTransitionTime
    float t = fmin(colorTransitionTime, 1.0f);

    // Interpolate between the colors
    Color fromColor = dayNightColors[fromColorIndex];
    Color toColor = dayNightColors[toColorIndex];
    Color currentColor = ColorLerp(fromColor, toColor, t);

    // Draw the day/night color overlay with the scaled dimensions
    DrawRectangle(baseX, baseY, BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT, currentColor);

    // Draw moving clouds
    DrawMovingCloudAndStar(deltaTime);

    // Draw day/night cycle debug overlay
    if (options->showDebug && debugToolToggles.showObjects)
    {
        DrawRectangle(baseX + 500, baseY + 25, 400, 20, Fade(GRAY, 0.7));
        DrawTextEx(meowFont, TextFormat("Time %.2f/%.2f | Phrase %d/%d", colorTransitionTime * dayNightCycleDuration, dayNightCycleDuration, currentColorIndex + 1, (sizeof(dayNightColors) / sizeof(dayNightColors[0]))), (Vector2) { baseX + BASE_SCREEN_WIDTH - 500, baseY + 25 }, 20, 2, WHITE);
    }

    // Update the colorTransitionTime
    if (colorTransitionTime >= 1.0f)
    {
        currentColorIndex = toColorIndex;
        colorTransitionTime = 0;
    }
    else
    {
        colorTransitionTime += GetFrameTime() * colorTransitionSpeed;
    }
}

void OptionsUpdate(Camera2D* camera)
{
    int currentHoveredButton = NO_BUTTON;
    Rectangle difficultyRect = { baseX + 780, baseY + 595, 340, 70 };
    Rectangle difficultyDecrementRect = { difficultyRect.x, difficultyRect.y, 60, 70 };
    Rectangle difficultyIncrementRect = { difficultyRect.x + 280, difficultyRect.y, 60, 70 };
    
    Rectangle resolutionRect = { baseX + 780, baseY + 675, 340, 70 };
    Rectangle resolutionDecrementRect = { resolutionRect.x, resolutionRect.y, 60, 70 };
    Rectangle resolutionIncrementRect = { resolutionRect.x + 280, resolutionRect.y, 60, 70 };

    Rectangle fpsRect = { baseX + 1200, baseY + 595, 340, 70 };
    Rectangle fpsDecrementRect = { fpsRect.x, fpsRect.y, 60, 70 };
    Rectangle fpsIncrementRect = { fpsRect.x + 280, fpsRect.y, 60, 70 };

    Rectangle musicRect = { baseX + 100, baseY + 595, 200, 70 };
    Rectangle soundFxRect = { baseX + 100, baseY + 675, 250, 70 };

    Rectangle fullscreenRect = { baseX + 400, baseY + 595, 300, 70 };
    Rectangle debugRect = { baseX + 400, baseY + 675, 200, 70 };

    Rectangle backRect = { baseX + 100, baseY + 840, 200, 70 };

    bool firstRender = true;
    double lastFrameTime = GetTime();

    bool isHovering = false;

    float alpha = 0.0f;
    double fadeInDuration = 0.35;
    double fadeOutDuration = 0.35;
    bool isFadingIn = true;
    bool isFadingOut = false;

    PlayBgmIfStopped(&menuBgm);

    while (!WindowShouldClose())
    {
        // Calculate delta time
        double deltaTime = GetTime() - lastFrameTime;
        lastFrameTime = GetTime();

        WindowUpdate(camera);

        if (isFadingIn)
		{
			alpha += deltaTime / fadeInDuration;
			if (alpha >= 1.0f)
			{
				alpha = 1.0f;
				isFadingIn = false;
			}
		}

	    if (isFadingOut)
        {
            alpha -= deltaTime / fadeOutDuration;
            if (alpha <= 0.0f)
            {
                alpha = 0.0f;
                isFadingOut = false;
                MainMenuUpdate(camera, false);
            }
        }

        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), *camera);

        bool isDifficultyIncrementHovered = CheckCollisionPointRec(mouseWorldPos, difficultyIncrementRect);
        bool isDifficultyDecrementHovered = CheckCollisionPointRec(mouseWorldPos, difficultyDecrementRect);
        
        bool isResolutionIncrementHovered = CheckCollisionPointRec(mouseWorldPos, resolutionIncrementRect);
        bool isResolutionDecrementHovered = CheckCollisionPointRec(mouseWorldPos, resolutionDecrementRect);

        bool isFpsIncrementHovered = CheckCollisionPointRec(mouseWorldPos, fpsIncrementRect);
        bool isFpsDecrementHovered = CheckCollisionPointRec(mouseWorldPos, fpsDecrementRect);
        
        bool isMusicHovered = CheckCollisionPointRec(mouseWorldPos, musicRect);
        bool isSoundFxHovered = CheckCollisionPointRec(mouseWorldPos, soundFxRect);
        bool isFullscreenHovered = CheckCollisionPointRec(mouseWorldPos, fullscreenRect);
        bool isDebugHovered = CheckCollisionPointRec(mouseWorldPos, debugRect);

        bool isBackHovered = CheckCollisionPointRec(mouseWorldPos, backRect);

        // Handle user input
        if (!firstRender && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !isFadingIn && !isFadingOut) {
            if (isDifficultyIncrementHovered) {
                if (options->difficulty == EASY) {
					options->difficulty = FREEPLAY_EASY;
				}
                else if (options->difficulty == FREEPLAY_EASY) {
                    options->difficulty = MEDIUM;
                }
                else if (options->difficulty == MEDIUM) {
                    options->difficulty = FREEPLAY_MEDIUM;
                }
                else if (options->difficulty == FREEPLAY_MEDIUM) {
                    options->difficulty = HARD;
                }
                else if (options->difficulty == HARD) {
                    options->difficulty = FREEPLAY_HARD;
                }
                PlaySoundFx(FX_SELECT);
            }
            else if (isDifficultyDecrementHovered) {
                if (options->difficulty == FREEPLAY_HARD) {
                    options->difficulty = HARD;
                }
                else if (options->difficulty == HARD) {
                    options->difficulty = FREEPLAY_MEDIUM;
                }
                else if (options->difficulty == FREEPLAY_MEDIUM) {
                    options->difficulty = MEDIUM;
                }
				else if (options->difficulty == MEDIUM) {
                    options->difficulty = FREEPLAY_EASY;
				}
                else if (options->difficulty == FREEPLAY_EASY) {
                    options->difficulty = EASY;
				}
                PlaySoundFx(FX_SELECT);
			}
            else if (isResolutionIncrementHovered) {
                // 720p -> 1080p
                if (options->resolution.x == 1280) {
                    options->resolution.x = 1920;
                    options->resolution.y = 1080;
                }
                // Custom resize resolution
				else
				{
                    // If current resolution is less than 720p, set it to 720p
                    if (options->resolution.x < 1280 || options->resolution.y < 720) {
						options->resolution.x = 1280;
						options->resolution.y = 720;
					}
				}
                SetRuntimeResolution(camera, options->resolution.x, options->resolution.y);
                PlaySoundFx(FX_SELECT);
            }
            else if (isResolutionDecrementHovered) {
                // 1080p -> 720p
                if (options->resolution.x == 1920) {
					options->resolution.x = 1280;
					options->resolution.y = 720;
				}
                // Custom resize resolution
                else {
                    // If current resolution is more than 1080p, set it to 1080p
                    if (options->resolution.x > 1920 || options->resolution.y > 1080) {
                        options->resolution.x = 1920;
                        options->resolution.y = 1080;
                    }
                }
                SetRuntimeResolution(camera, options->resolution.x, options->resolution.y);
                PlaySoundFx(FX_SELECT);
            }
            else if (isFpsIncrementHovered) {
                // Change target FPS 30, 60, 120, 144, 240, Basically Unlimited (1000)
                if (options->targetFps == 30) {
                    options->targetFps = 60;
                }
                else if (options->targetFps == 60) {
                    options->targetFps = 120;
                }
                else if (options->targetFps == 120) {
                    options->targetFps = 144;
                }
                else if (options->targetFps == 144) {
                    options->targetFps = 240;
                }
                else if (options->targetFps == 240) {
                    options->targetFps = 1000;
                }

                SetTargetFPS(options->targetFps);
                PlaySoundFx(FX_SELECT);
            }
            else if (isFpsDecrementHovered) {
                // Change target FPS 30, 60, 120, 144, 240, Basically Unlimited (1000)
                if (options->targetFps == 60) {
                    options->targetFps = 30;
                }
                else if (options->targetFps == 120) {
                    options->targetFps = 60;
                }
                else if (options->targetFps == 144) {
                    options->targetFps = 120;
                }
                else if (options->targetFps == 240) {
                    options->targetFps = 144;
                }
                else if (options->targetFps == 1000) {
					options->targetFps = 240;
				}

                SetTargetFPS(options->targetFps);
                PlaySoundFx(FX_SELECT);
            }
            else if (isFullscreenHovered) {
                // Toggle fullscreen
                options->fullscreen = !options->fullscreen;
                SetRuntimeResolution(camera, options->resolution.x, options->resolution.y);
                ToggleFullscreen();
                PlaySoundFx(FX_SELECT);
            }
            else if (isDebugHovered) {
				// Toggle debug
				options->showDebug = !options->showDebug;
                PlaySoundFx(FX_SELECT);
			}
			else if (isBackHovered) {
				// Go back to main menu
                isFadingOut = true;
                PlaySoundFx(FX_SELECT);
			}
		    else if (isMusicHovered) {
				// Toggle music
				options->musicEnabled = !options->musicEnabled;
                PlaySoundFx(FX_SELECT);
                if (options->musicEnabled)
                    PlayBgm(&menuBgm);
				else
                    PauseBgm(&menuBgm);
			}
			else if (isSoundFxHovered) {
				// Toggle sound fx
				options->soundFxEnabled = !options->soundFxEnabled;
                PlaySoundFx(FX_SELECT);
			}
        }

        // Play sound when hovering over a button, but only once
        if (!isFadingIn && !isFadingOut)
        {
            if (isDifficultyIncrementHovered)
            {
                if (!isHovering || currentHoveredButton != 0)
                {
                    PlaySoundFx(FX_HOVER);
                    isHovering = true;
                }
                currentHoveredButton = 0;
            }
            else if (isDifficultyDecrementHovered)
            {
                if (!isHovering || currentHoveredButton != 1)
                {
                    PlaySoundFx(FX_HOVER);
                    isHovering = true;
                }
                currentHoveredButton = 1;
            }
            else if (isResolutionIncrementHovered)
            {
                if (!isHovering || currentHoveredButton != 2)
                {
                    PlaySoundFx(FX_HOVER);
                    isHovering = true;
                }
                currentHoveredButton = 2;
            }
            else if (isResolutionDecrementHovered)
            {
                if (!isHovering || currentHoveredButton != 3)
                {
                    PlaySoundFx(FX_HOVER);
                    isHovering = true;
                }
                currentHoveredButton = 3;
            }
            else if (isFpsIncrementHovered)
            {
                if (!isHovering || currentHoveredButton != 4)
                {
                    PlaySoundFx(FX_HOVER);
                    isHovering = true;
                }
                currentHoveredButton = 4;
            }
            else if (isFpsDecrementHovered)
            {
                if (!isHovering || currentHoveredButton != 5)
                {
                    PlaySoundFx(FX_HOVER);
                    isHovering = true;
                }
                currentHoveredButton = 5;
            }
            else if (isFullscreenHovered)
            {
                if (!isHovering || currentHoveredButton != 6)
                {
                    PlaySoundFx(FX_HOVER);
                    isHovering = true;
                }
                currentHoveredButton = 6;
            }
            else if (isDebugHovered)
            {
                if (!isHovering || currentHoveredButton != 7)
                {
                    PlaySoundFx(FX_HOVER);
                    isHovering = true;
                }
                currentHoveredButton = 7;
            }
            else if (isBackHovered)
            {
                if (!isHovering || currentHoveredButton != 8)
                {
                    PlaySoundFx(FX_HOVER);
                    isHovering = true;
                }
                currentHoveredButton = 8;
            }
            else if (isMusicHovered)
            {
                if (!isHovering || currentHoveredButton != 9)
                {
                    PlaySoundFx(FX_HOVER);
                    isHovering = true;
                }
                currentHoveredButton = 9;
            }
            else if (isSoundFxHovered)
            {
                if (!isHovering || currentHoveredButton != 10)
                {
                    PlaySoundFx(FX_HOVER);
                    isHovering = true;
                }
                currentHoveredButton = 10;
            }
            else
            {
                currentHoveredButton = NO_BUTTON;  // Use NO_BUTTON instead of NULL
                isHovering = false;
            }
        }
        else
        {
            isDifficultyIncrementHovered = false;
            isDifficultyDecrementHovered = false;
            isResolutionIncrementHovered = false;
            isResolutionDecrementHovered = false;
            isFpsIncrementHovered = false;
            isFpsDecrementHovered = false;
            isMusicHovered = false;
            isSoundFxHovered = false;
            isFullscreenHovered = false;
            isDebugHovered = false;
            isBackHovered = false;
        }
        if (firstRender)
            firstRender = false;

        // Draw

        BeginDrawing();
        ClearBackground(BLACK);


        BeginMode2D(*camera);

        int imageWidth = backgroundTexture.width;
        int imageHeight = backgroundTexture.height;

        float scaleX = (float)BASE_SCREEN_WIDTH / imageWidth;
        float scaleY = (float)BASE_SCREEN_HEIGHT / imageHeight;

        // Draw the background with the scaled dimensions
        //DrawTextureEx(backgroundTexture, (Vector2) { baseX, baseY }, 0.0f, fmax(scaleX, scaleY), WHITE);

        DrawDayNightCycle(deltaTime);

        // Draw falling items behind the menu
        DrawMenuFallingItems(deltaTime, true);

        DrawCustomerInMenu(deltaTime);

        DrawTextureEx(backgroundOverlayTexture, (Vector2) { baseX, baseY }, 0.0f, fmax(scaleX, scaleY), WHITE);

        // Draw falling items in front of the menu
        DrawMenuFallingItems(deltaTime, false);

        // Music
        DrawTextureEx(options->musicEnabled ? checkboxChecked : checkbox, (Vector2) { musicRect.x + 10, musicRect.y + 10 }, 0.0f, 1.0f / 6.0f, ColorAlphaOverride(WHITE, isMusicHovered ? 0.75 : alpha));
		DrawTextEx(meowFont, "Music", (Vector2) { musicRect.x + 80, musicRect.y + 22 }, 32, 2, ColorAlphaOverride(MAIN_BROWN, isMusicHovered ? 0.75 : alpha));

        // Sound FX
        DrawTextureEx(options->soundFxEnabled ? checkboxChecked : checkbox, (Vector2) { soundFxRect.x + 10, soundFxRect.y + 10 }, 0.0f, 1.0f / 6.0f, ColorAlphaOverride(WHITE, isSoundFxHovered ? 0.75 : alpha));
        DrawTextEx(meowFont, "Sound FX", (Vector2) { soundFxRect.x + 80, soundFxRect.y + 22 }, 32, 2, ColorAlphaOverride(MAIN_BROWN, isSoundFxHovered ? 0.75 : alpha));

        // Fullscreen
        DrawTextureEx(options->fullscreen ? checkboxChecked : checkbox, (Vector2) { fullscreenRect.x + 10, fullscreenRect.y + 10 }, 0.0f, 1.0f / 6.0f, ColorAlphaOverride(WHITE, isFullscreenHovered ? 0.75 : alpha));
        DrawTextEx(meowFont, "Fullscreen", (Vector2) { fullscreenRect.x + 80, fullscreenRect.y + 22 }, 32, 2, ColorAlphaOverride(MAIN_BROWN, isFullscreenHovered ? 0.75 : alpha));

        // Debug
        DrawTextureEx(options->showDebug ? checkboxChecked : checkbox, (Vector2) { debugRect.x + 10, debugRect.y + 10 }, 0.0f, 1.0f / 6.0f, ColorAlphaOverride(WHITE, isDebugHovered ? 0.75 : alpha));
		DrawTextEx(meowFont, "Debug", (Vector2) { debugRect.x + 80, debugRect.y + 22 }, 32, 2, ColorAlphaOverride(MAIN_BROWN, isDebugHovered ? 0.75 : alpha));

        // Difficulty
        DrawTextureEx(left_arrow, (Vector2) { difficultyDecrementRect.x , difficultyDecrementRect.y}, 0.0f, 1.0f / 5.0f, ColorAlphaOverride(WHITE, isDifficultyDecrementHovered ? 0.75 : alpha));
        DrawTextureEx(right_arrow, (Vector2) { difficultyIncrementRect.x, difficultyIncrementRect.y }, 0.0f, 1.0f / 5.0f, ColorAlphaOverride(WHITE, isDifficultyIncrementHovered ? 0.75 : alpha));
        DrawTextEx(meowFont, "Difficulty", (Vector2) { difficultyRect.x + 80, difficultyRect.y + 10 }, 32, 2, ColorAlphaOverride(MAIN_BROWN, alpha));
        DrawTextEx(meowFont, StringFromDifficultyEnum(options->difficulty), (Vector2) { difficultyRect.x + 80, difficultyRect.y + 42 }, 32, 2, ColorAlphaOverride(MAIN_BROWN, alpha));

        // Resolution
        DrawTextureEx(left_arrow, (Vector2) { resolutionDecrementRect.x, resolutionDecrementRect.y }, 0.0f, 1.0f / 5.0f, ColorAlphaOverride(WHITE, isResolutionDecrementHovered ? 0.75 : alpha));
        DrawTextureEx(right_arrow, (Vector2) { resolutionIncrementRect.x, resolutionDecrementRect.y }, 0.0f, 1.0f / 5.0f, ColorAlphaOverride(WHITE, isResolutionIncrementHovered ? 0.75 : alpha));
        DrawTextEx(meowFont, "Resolution", (Vector2) { resolutionRect.x + 80, resolutionRect.y + 10 }, 32, 2, ColorAlphaOverride(MAIN_BROWN, alpha));
        DrawTextEx(meowFont, TextFormat("%dx%d", options->resolution.x, options->resolution.y), (Vector2) { resolutionRect.x + 80, resolutionRect.y + 42 }, 32, 2, ColorAlphaOverride(MAIN_BROWN, alpha));

        // FPS
        DrawTextureEx(left_arrow, (Vector2) { fpsDecrementRect.x, fpsDecrementRect.y }, 0.0f, 1.0f / 5.0f, ColorAlphaOverride(WHITE, isFpsDecrementHovered ? 0.75 : alpha));
        DrawTextureEx(right_arrow, (Vector2) { fpsIncrementRect.x, fpsDecrementRect.y }, 0.0f, 1.0f / 5.0f, ColorAlphaOverride(WHITE, isFpsIncrementHovered ? 0.75 : alpha));
		DrawTextEx(meowFont, "Target FPS", (Vector2) { fpsRect.x + 80, fpsRect.y + 10 }, 32, 2, ColorAlphaOverride(MAIN_BROWN, alpha));
        DrawTextEx(meowFont, TextFormat("%d FPS", options->targetFps), (Vector2) { fpsRect.x + 80, fpsRect.y + 42 }, 32, 2, ColorAlphaOverride(MAIN_BROWN, alpha));


        // Back
        DrawRectangleRec(backRect, isBackHovered ? ColorAlphaOverride(MAIN_ORANGE, alpha) : ColorAlphaOverride(MAIN_BROWN, alpha));
        DrawTextEx(meowFont, "Back", (Vector2) { backRect.x + 60, backRect.y + 20 }, 32, 2, ColorAlphaOverride(WHITE, alpha));

        // Draw debug
        if (options->showDebug && debugToolToggles.showObjects)
        {
            DrawRectangleLinesEx(difficultyRect, 1, RED);
            DrawRectangleLinesEx(difficultyDecrementRect, 1, RED);
            DrawRectangleLinesEx(difficultyIncrementRect, 1, RED);
            DrawRectangleLinesEx(resolutionRect, 1, RED);
            DrawRectangleLinesEx(resolutionDecrementRect, 1, RED);
            DrawRectangleLinesEx(resolutionIncrementRect, 1, RED);
            DrawRectangleLinesEx(fpsRect, 1, RED);
            DrawRectangleLinesEx(fpsDecrementRect, 1, RED);
            DrawRectangleLinesEx(fpsIncrementRect, 1, RED);

            DrawRectangleLinesEx(musicRect, 1, RED);
            DrawRectangleLinesEx(soundFxRect, 1, RED);
            DrawRectangleLinesEx(fullscreenRect, 1, RED);
            DrawRectangleLinesEx(debugRect, 1, RED);
        }

        DrawOuterWorld();

        if (options->showDebug)
            DrawDebugOverlay(camera);

        EndMode2D();
        EndDrawing();
    }

    ExitApplication();
}

int RandomCustomerTexture()
{
    int randomIndex = rand() % (sizeof(customersImageData) / sizeof(customersImageData[0]));
    return randomIndex;
}
void GameUpdate(Camera2D *camera)
{
    double lastFrameTime = GetTime();
    bool isDragging = false;
    bool isDraggingOnce = false;

    bool isHovering = false;
    bool hoversoundPlayed = false;

    // Start from night
    currentColorIndex = 3;

    dayNightCycleDuration = gameDuration * 3;
    colorTransitionTime = 0.2f;

    bool passedInitialPhrase = false;

    Cup cup = {
        LoadTexture(ASSETS_PATH"combination/EMPTY.png"),
        (Vector2) {0, 0},
        NONE,
        false,
        NONE,
        NONE,
        NONE,
        false,
        '\0'
    };

    plate = (DropArea){ LoadTexture(ASSETS_PATH"/spritesheets/MAT.png"), oriplatePosition };
    Texture2D cups = LoadTexture(ASSETS_PATH"/spritesheets/CUPS.png");

    teaPowder = (Ingredient){ teaPowderTexture, true, oriteapowderPosition, oriteapowderPosition };
    teaPowder.totalFrames = 3;
    teaPowder.currentFrame = 1;
    teaPowder.frameRectangle = frameRect(teaPowder, teaPowder.totalFrames, teaPowder.currentFrame);

    cocoaPowder = (Ingredient){ cocoaPowderTexture, true, oricocoapowderPosition, oricocoapowderPosition };
    cocoaPowder.totalFrames = 3;
    cocoaPowder.currentFrame = 1;
    cocoaPowder.frameRectangle = frameRect(cocoaPowder, cocoaPowder.totalFrames, cocoaPowder.currentFrame);

    caramelSauce = (Ingredient){ caramelSauceTexture, true, oricaramelPosition, oricaramelPosition };
    caramelSauce.totalFrames = 3;
    caramelSauce.currentFrame = 1;
    caramelSauce.frameRectangle = frameRect(caramelSauce, caramelSauce.totalFrames, caramelSauce.currentFrame);

    chocolateSauce = (Ingredient){ chocolateSauceTexture, true, orichocolatePosition, orichocolatePosition };
    chocolateSauce.totalFrames = 3;
    chocolateSauce.currentFrame = 1;
    chocolateSauce.frameRectangle = frameRect(chocolateSauce, chocolateSauce.totalFrames, chocolateSauce.currentFrame);

    condensedMilk = (Ingredient){ condensedMilkTexture, true, oricondensedmilkPosition, oricondensedmilkPosition };
    condensedMilk.totalFrames = 2;
    condensedMilk.currentFrame = 1;
    condensedMilk.frameRectangle = frameRect(condensedMilk, condensedMilk.totalFrames, condensedMilk.currentFrame);

    normalMilk = (Ingredient){ normalMilkTexture, true, orimilkPosition, orimilkPosition };
    normalMilk.totalFrames = 2;
    normalMilk.currentFrame = 1;
    normalMilk.frameRectangle = frameRect(normalMilk, normalMilk.totalFrames, normalMilk.currentFrame);

    marshMellow = (Ingredient){ marshMellowTexture, true, orimarshmellowPosition, orimarshmellowPosition };
    marshMellow.totalFrames = 2;
    marshMellow.currentFrame = 1;
    marshMellow.frameRectangle = frameRect(marshMellow, marshMellow.totalFrames, marshMellow.currentFrame);

    whippedCream = (Ingredient){ whippedCreamTexture, true, oriwhippedPosition, oriwhippedPosition };
    whippedCream.totalFrames = 2;
    whippedCream.currentFrame = 1;
    whippedCream.frameRectangle = frameRect(whippedCream, whippedCream.totalFrames, whippedCream.currentFrame);

    hotWater = (Ingredient){ hotWaterTexture, true, oriwaterPosition, oriwaterPosition };
    hotWater.totalFrames = 17;
    hotWater.currentFrame = 1;
    hotWater.frameRectangle = frameRect(hotWater, hotWater.totalFrames, hotWater.currentFrame);
    hotWater.canChangeCupTexture = false;

    greenChon = (Ingredient){ greenChonTexture, false, hiddenPosition, hiddenPosition };
    greenChon.totalFrames = 1;
    greenChon.currentFrame = 1;
    greenChon.frameRectangle = frameRect(greenChon, greenChon.totalFrames, greenChon.currentFrame);

    cocoaChon = (Ingredient){ cocoaChonTexture, false, hiddenPosition, hiddenPosition };
    cocoaChon.totalFrames = 1;
    cocoaChon.currentFrame = 1;
	cocoaChon.frameRectangle = frameRect(cocoaChon, cocoaChon.totalFrames, cocoaChon.currentFrame);


    trashCan = (Ingredient){ trashCanTexture, false, trashCanPosition, trashCanPosition };
    trashCan.currentFrame = 1;
    trashCan.totalFrames = 2;
    trashCan.frameRectangle = frameRect(trashCan, trashCan.totalFrames, trashCan.currentFrame);

    Texture2D* currentDrag = NULL;

    Customer customer1;
    Customer customer2;
    Customer customer3;

    Customers customers;

    Vector2 customer1Position = { baseX + 50, baseY + 100 };
    Vector2 customer2Position = { baseX + 650, baseY + 100 };
    Vector2 customer3Position = { baseX + 1250, baseY + 100 };

    Rectangle endScene = { 770, -500, 140, 70 };

    double initialReset[3];
    RandomCustomerInitialResetBasedOnDifficulty(&initialReset);

    customer1 = CreateCustomerWithOrder(1, 0, RandomCustomerTimeoutBasedOnDifficulty(), customer1Position, RandomCustomerTexture(), initialReset[0]);
    customer2 = CreateCustomerWithOrder(1, 0, RandomCustomerTimeoutBasedOnDifficulty(), customer2Position, RandomCustomerTexture(), initialReset[1]);
    customer3 = CreateCustomerWithOrder(1, 0, RandomCustomerTimeoutBasedOnDifficulty(), customer3Position, RandomCustomerTexture(), initialReset[2]);
    customer1.visible = false;
    customer2.visible = false;
    customer3.visible = false;

    customers.customer1 = customer1;
    customers.customer2 = customer2;
    customers.customer3 = customer3;
    
    while (!WindowShouldClose())
    {
        // Calculate delta time
        double deltaTime = GetTime() - lastFrameTime;
        lastFrameTime = GetTime();

         // Esc key to return to main menu
        if (IsKeyPressed(KEY_ESCAPE))
        {
            ResetGameState();
            MainMenuUpdate(camera, false);
            return;
        }

        WindowUpdate(camera);

        bool anyDragDetected = false;

        if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            dragAndDropLocked = false;
        }

        if (!dragAndDropLocked)
        {
            // Dragable items
            if (currentDrag == NULL || currentDrag == &teaPowder.texture) {
                currentDrag = DragAndDropIngredientPop(&teaPowder, &greenChon, &cup, camera);
                if (currentDrag != NULL) anyDragDetected = true;
            }if (currentDrag == NULL || currentDrag == &cocoaPowder.texture) {
                // currentDrag = DragAndDropIngredient(&cocoaPowder, &plate, &cup, camera);
                currentDrag = DragAndDropIngredientPop(&cocoaPowder, &cocoaChon, &cup, camera);
                if (currentDrag != NULL) anyDragDetected = true;
            }if (currentDrag == NULL || currentDrag == &condensedMilk.texture) {
                currentDrag = DragAndDropIngredient(&condensedMilk, &cup, camera);
                if (currentDrag != NULL) anyDragDetected = true;
            }if (currentDrag == NULL || currentDrag == &normalMilk.texture) {
                currentDrag = DragAndDropIngredient(&normalMilk, &cup, camera);
                if (currentDrag != NULL) anyDragDetected = true;
            }if (currentDrag == NULL || currentDrag == &whippedCream.texture) {
                currentDrag = DragAndDropIngredient(&whippedCream, &cup, camera);
                if (currentDrag != NULL) anyDragDetected = true;
            }if (currentDrag == NULL || currentDrag == &marshMellow.texture) {
                currentDrag = DragAndDropIngredient(&marshMellow, &cup, camera);
                if (currentDrag != NULL) anyDragDetected = true;
            }if (currentDrag == NULL || currentDrag == &caramelSauce.texture) {
                currentDrag = DragAndDropIngredient(&caramelSauce, &cup, camera);
                if (currentDrag != NULL) anyDragDetected = true;
            }if (currentDrag == NULL || currentDrag == &chocolateSauce.texture) {
                currentDrag = DragAndDropIngredient(&chocolateSauce, &cup, camera);
                if (currentDrag != NULL) anyDragDetected = true;
            }if (currentDrag == NULL || currentDrag == &hotWater.texture) {
                currentDrag = DragAndDropIngredient(&hotWater, &cup, camera);
                if (currentDrag != NULL) anyDragDetected = true;
            }
            if (currentDrag == NULL || currentDrag == &cup.texture) {
                currentDrag = DragAndDropCup(&cup, &plate, camera, &customers, &trashCan);
                if (currentDrag != NULL) anyDragDetected = true;
            }
        }

        

        if (!dragAndDropLocked && !anyDragDetected && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            dragAndDropLocked = true;
        }

        bool previousIsDragging = isDragging;

        // If something is dragged now, play sound
        if (currentDrag != NULL && !isDraggingOnce)
		{
            PlaySoundFx(FX_PICKUP);
            isDraggingOnce = true;
		}
		else if (currentDrag == NULL)
		{
            if(previousIsDragging != isDraggingOnce)
                PlaySoundFx(FX_DROP);

            isDraggingOnce = false;
		}

        isHovering = false;
        // check mouse not down
        if (IsMouseButtonUp(MOUSE_LEFT_BUTTON)) {
            // call highlightItem for each item
            isHovering = highlightItem(&teaPowder, camera) || isHovering;
            isHovering = highlightItem(&cocoaPowder, camera) || isHovering;
            isHovering = highlightItem(&condensedMilk, camera) || isHovering;
            isHovering = highlightItem(&normalMilk, camera) || isHovering;
            isHovering = highlightItem(&whippedCream, camera) || isHovering;
            isHovering = highlightItem(&marshMellow, camera) || isHovering;
            isHovering = highlightItem(&caramelSauce, camera) || isHovering;
            isHovering = highlightItem(&chocolateSauce, camera) || isHovering;
            isHovering = highlightItem(&hotWater, camera) || isHovering;
            isHovering = highlightItem(&trashCan,camera) || isHovering;
        }
        else {
            isHovering = false;
        }
        if (isHovering && !hoversoundPlayed) {

            hoversoundPlayed = true;
            //PlaySound(hover);

        }
        else if (!isHovering) {
            hoversoundPlayed = false;
        }

        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), *camera);
        if (CheckCollisionPointRec(mousePos, (Rectangle) { cup.position.x, cup.position.y, cup.frameRectangle.width, cup.frameRectangle.height })) {
            cup.frameRectangle = frameRectCup(cup, 2, 2);
        }
        else {
            cup.frameRectangle = frameRectCup(cup, 2, 1);
        }

        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), *camera);
        bool isendSceneHovered = CheckCollisionPointRec(mouseWorldPos, endScene);
        void (*transitionCallback)(Camera2D * camera) = NULL;

        if (!passedInitialPhrase && currentColorIndex == 0)
            passedInitialPhrase = true;

        // Day ended, show end scene
        if (passedInitialPhrase && currentColorIndex >= 2)
        {
            // If not freeplay
            if (options->difficulty != FREEPLAY_EASY && options->difficulty != FREEPLAY_MEDIUM && options->difficulty != FREEPLAY_HARD)
			{
                endgameUpdate(camera);
			}
        }
        
        Tick(&customers, deltaTime);
        tickBoil(&hotWater);

        // Draw

        BeginDrawing();
        ClearBackground(BLACK);


        BeginMode2D(*camera);
        

        int imageWidth = backgroundTexture.width;
        int imageHeight = backgroundTexture.height;

        float scaleX = (float)BASE_SCREEN_WIDTH / imageWidth;
        float scaleY = (float)BASE_SCREEN_HEIGHT / imageHeight;

        DrawDayNightCycle(deltaTime);

		render_customers(&customers);

        DrawTextureEx(backgroundOverlayTexture, (Vector2) { baseX, baseY }, 0.0f, fmax(scaleX, scaleY), WHITE);

        DrawTexture(plate.texture, oriplatePosition.x, oriplatePosition.y, WHITE);

        DrawDragableItemFrame(trashCan);


        DrawDragableItemFrame(hotWater);
        DrawDragableItemFrame(condensedMilk);
        DrawDragableItemFrame(normalMilk);
        DrawTexture(cups, oricupsPostion.x, oricupsPostion.y, WHITE);

        DrawDragableItemFrame(cocoaPowder);
        DrawDragableItemFrame(teaPowder);

        DrawDragableItemFrame(chocolateSauce);
        DrawDragableItemFrame(caramelSauce);

        DrawDragableItemFrame(marshMellow);
        DrawDragableItemFrame(whippedCream);

        // DrawTexture(cup.texture, cup.position.x, cup.position.y, WHITE);

        DrawTexture(greenChon.texture, greenChon.position.x, greenChon.position.y, WHITE);
        DrawTexture(cocoaChon.texture, cocoaChon.position.x, cocoaChon.position.y, WHITE);
        

        if (cup.active)
            DrawTextureRec(cup.texture, cup.frameRectangle, cup.position, WHITE);

        // Draw debug for cup
        if (options->showDebug && debugToolToggles.showObjects)
        {
            DrawRectangleLinesEx((Rectangle) { cup.position.x, cup.position.y, cup.frameRectangle.width, cup.frameRectangle.height }, 1, RED);
            DrawRectangle(cup.position.x, cup.position.y - 60, 400, 60, Fade(GRAY, 0.7));
            DrawTextEx(meowFont, TextFormat("Powder type: %d, Water: %d, Creamer: %d", cup.powderType, cup.hasWater, cup.creamerType), (Vector2) { cup.position.x, cup.position.y - 20 }, 20, 1, WHITE);
            DrawTextEx(meowFont, TextFormat("Topping: %d, Sauce: %d", cup.toppingType, cup.sauceType), (Vector2) { cup.position.x, cup.position.y - 40 }, 20, 1, WHITE);
			DrawTextEx(meowFont, TextFormat("%s | XY %.2f,%.2f | Active %s", "Cup", cup.position.x, cup.position.y, cup.active ? "[Yes]" : "[No]"), (Vector2) { cup.position.x, cup.position.y - 60 }, 20, 1, WHITE);
            
            // Cups
            DrawRectangleLinesEx((Rectangle) { oricupsPostion.x, oricupsPostion.y, cups.width, cups.height }, 1, RED);
            DrawRectangle(oricupsPostion.x, oricupsPostion.y - 20, 400, 20, Fade(GRAY, 0.7));
            DrawTextEx(meowFont, TextFormat("%s | XY %.2f,%.2f | Grabbable %s", "Cups", oricupsPostion.x, oricupsPostion.y, cup.active ? "[No]" : "[Yes]"), (Vector2) { oricupsPostion.x, oricupsPostion.y - 20 }, 20, 1, WHITE);

            // Plate
            DrawRectangleLinesEx((Rectangle) { oriplatePosition.x, oriplatePosition.y, plate.texture.width, plate.texture.height }, 1, RED);
            DrawRectangle(oriplatePosition.x, oriplatePosition.y - 20, 300, 20, Fade(GRAY, 0.7));
            DrawTextEx(meowFont, TextFormat("%s | XY %.2f,%.2f", "Plate", oriplatePosition.x, oriplatePosition.y), (Vector2) { oriplatePosition.x, oriplatePosition.y - 20 }, 20, 1, WHITE);
        }

		/* Customers TEST AREA END*/
        if(&customers.customer1 != NULL)
            UpdateMenuCustomerBlink(&customers.customer1, deltaTime);
        if (&customers.customer2 != NULL)
            UpdateMenuCustomerBlink(&customers.customer2, deltaTime);
        if (&customers.customer3 != NULL)
            UpdateMenuCustomerBlink(&customers.customer3, deltaTime);


        DrawTextEx(meowFont, TextFormat("Score: %d", global_score), (Vector2) { baseX + 20, baseY + 20 }, 26, 2, WHITE);

        DrawOuterWorld();

        if (options->showDebug)
            DrawDebugOverlay(camera);

        // End game
        if(options->difficulty == FREEPLAY_EASY || options->difficulty == FREEPLAY_MEDIUM || options->difficulty == FREEPLAY_HARD)
		{
            DrawRectangleRec(endScene, ColorAlphaOverride(RED, isendSceneHovered ? 0.5f : 1.0f));
            DrawTextEx(meowFont, "End", (Vector2) { endScene.x + 42, endScene.y + 22 }, 32, 2, ColorAlphaOverride(WHITE, isendSceneHovered ? 0.5f : 1.0f));

			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (isendSceneHovered))
			{
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (isendSceneHovered) {
                        endgameUpdate(camera);
                    }
                }

			}
		}

        EndMode2D();
        EndDrawing();
    }

    UnloadTexture(backgroundTexture);
    CloseWindow();

}

void ResetGameState()
{
    global_score = 0;

    // Reset hotWater boiling
    triggerHotWater = false;
    hotWater.canChangeCupTexture = false;
    hotWater.currentFrame = 1;
}

void endgameUpdate(Camera2D *camera){

    int imageWidth = backgroundTexture.width;
    int imageHeight = backgroundTexture.height;

    float scaleX = (float)BASE_SCREEN_WIDTH / imageWidth;
    float scaleY = (float)BASE_SCREEN_HEIGHT / imageHeight;

    Rectangle scoreRec = {-277,-241,600,450};
    float centerX = scoreRec.x + (scoreRec.width / 2);
    float centerY = scoreRec.y + (scoreRec.height / 2);
    Rectangle tryagain = { centerX - 90 , centerY + 50, 200, 70 };


    while (!WindowShouldClose()){
        WindowUpdate(camera);
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), *camera);
        bool istryagainHovered = CheckCollisionPointRec(mouseWorldPos, tryagain);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(*camera);

        DrawTextureEx(backgroundTexture, (Vector2) { baseX, baseY }, 0.0f, fmax(scaleX, scaleY), WHITE);
        DrawRectangleRec(scoreRec, MAIN_BROWN);
        DrawRectangleLinesEx((Rectangle) {-217,-195, 480, 360}, 5, WHITE);

        Vector2 scorePos;
        scorePos.x = centerX - (MeasureText("Score", 55) / 2);
        scorePos.y = centerY - 150; 
        DrawTextEx(meowFont, "Score", scorePos, 55, 2, WHITE);

        char *scoreText = TextFormat("%d", global_score);
        Vector2 scoreTextPos;
        scoreTextPos.x = centerX - (MeasureText(scoreText, 100) / 2);
        scoreTextPos.y = centerY - 80; 
        DrawTextEx(meowFont, scoreText, scoreTextPos, 100, 2, WHITE);

        DrawRectangleRec(tryagain, ColorAlphaOverride(RED, istryagainHovered ? 0.5f : 1.0f));
        DrawTextEx(meowFont, "Menu", (Vector2) {-10,55}, 32, 2, ColorAlphaOverride(WHITE, istryagainHovered ? 0.5f : 1.0f));

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (istryagainHovered))
        {
            ResetGameState();
            MainMenuUpdate(camera, false);
        }

        if(options->showDebug)
            DrawDebugOverlay(camera);

        EndMode2D();
        EndDrawing();
    }
}

void MainMenuUpdate(Camera2D* camera, bool playFade)
{
    int currentHoveredButton = NO_BUTTON;
    float fadeOutDuration = 1.0f;
    double currentTime = 0;
    double lastFrameTime = GetTime();  // Add lastFrameTime declaration
    bool isHovering = false;
    void (*transitionCallback)(Camera2D* camera) = NULL;

    int splashBackgroundImageWidth = splashBackgroundTexture.width;
    int splashBackgroundImageHeight = splashBackgroundTexture.height;

    float splashBackgroundScaleX = (float)BASE_SCREEN_WIDTH / splashBackgroundImageWidth;
    float splashBackgroundScaleY = (float)BASE_SCREEN_HEIGHT / splashBackgroundImageHeight;

    float transitionOffset = 0;

    bool isFadeOutDone = false;
    bool isTransitioningIn = !playFade;
    bool isTransitioningOut = false;

    if(isTransitioningIn)
        transitionOffset = BASE_SCREEN_WIDTH / 2;

    trashCanPosition = (Vector2){baseX , baseY + BASE_SCREEN_HEIGHT - 200};

    PlayBgmIfStopped(&menuBgm);

    if(playFade)
    {
        movingClouds[0] = (MovingCloud){ (Vector2) { GetRandomDoubleValue(baseX - 200, baseX), GetRandomDoubleValue(baseY, 0) }, 250.0f, 1.0f, cloud1Texture, false };
        movingClouds[1] = (MovingCloud){ (Vector2) { GetRandomDoubleValue(baseX - 200, baseX), GetRandomDoubleValue(baseY, 0) }, 200.0f, 1.0f, cloud2Texture, false };
        movingClouds[2] = (MovingCloud){ (Vector2) { GetRandomDoubleValue(baseX - 200, baseX), GetRandomDoubleValue(baseY, 0) }, 150.0f, 2.0f, cloud3Texture, true };
        movingClouds[3] = (MovingCloud){ (Vector2) { GetRandomDoubleValue(baseX - 200, baseX), GetRandomDoubleValue(baseY, 0) }, 250.0f, 1.0f, cloud1Texture, false };
        movingClouds[4] = (MovingCloud){ (Vector2) { GetRandomDoubleValue(baseX - 200, baseX), GetRandomDoubleValue(baseY, 0) }, 100.0f, 1.7f, cloud2Texture, false };
        movingClouds[5] = (MovingCloud){ (Vector2) { GetRandomDoubleValue(baseX - 200, baseX), GetRandomDoubleValue(baseY, 0) }, 200.0f, 1.0f, cloud3Texture, true };
        movingClouds[6] = (MovingCloud){ (Vector2) { GetRandomDoubleValue(baseX - 200, baseX), GetRandomDoubleValue(baseY, 0) }, 150.0f, 1.0f, cloud3Texture, true };

        movingStars[0] = (MovingStar){ (Vector2) { GetRandomDoubleValue(baseX - 200, baseX), baseY }, 50.0f, 3.0f, star1Texture };
        movingStars[1] = (MovingStar){ (Vector2) { GetRandomDoubleValue(baseX - 200, baseX), baseY }, 25.0f, 2.0f, star2Texture };

        for (int i = 0; i < 20; i++) {
            menuFallingItems[i].position = (Vector2){ GetRandomDoubleValue(baseX, baseX + BASE_SCREEN_WIDTH - 20), baseY - GetRandomDoubleValue(200, 1000) };
            menuFallingItems[i].textureIndex = GetRandomValue(0, menuFallingItemsNumber - 1);

            // Random rotation and falling speed
            menuFallingItems[i].rotation = GetRandomDoubleValue(-360, 360);
            menuFallingItems[i].fallingSpeed = GetRandomDoubleValue(1, 3);
            menuFallingItems[i].fallingSpeed *= 100;

            // Rotation speed is between -3 and 3, but should not execeed falling speed and should not be 0
            menuFallingItems[i].rotationSpeed = GetRandomValue(-3, 3);
            if (fabsf(menuFallingItems[i].rotationSpeed) > menuFallingItems[i].fallingSpeed)
                menuFallingItems[i].rotationSpeed = menuFallingItems[i].fallingSpeed;
            if (menuFallingItems[i].rotationSpeed == 0)
                menuFallingItems[i].rotationSpeed = 1;

            menuFallingItems[i].rotationSpeed *= 100;

        }
    }

    while (!WindowShouldClose())
    {
        // Calculate delta time
        double deltaTime = GetTime() - lastFrameTime;
        lastFrameTime = GetTime();

        if (!isFadeOutDone)
            currentTime += GetFrameTime();

        WindowUpdate(camera);

        // Button positions and dimensions
        Rectangle startButtonRect = { baseX + 50 - transitionOffset, baseY + 600 + 60, 400, 100 };
        Rectangle optionsButtonRect = { baseX + 50 - transitionOffset, baseY + 600 + 160, 400, 100 };
        Rectangle exitButtonRect = { baseX + 50 - transitionOffset, baseY + 600 + 350, 400, 100 };

        // Convert mouse position from screen space to world space
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), *camera);

        bool isStartButtonHovered = CheckCollisionPointRec(mouseWorldPos, startButtonRect);
        bool isOptionsButtonHovered = CheckCollisionPointRec(mouseWorldPos, optionsButtonRect);
        bool isExitButtonHovered = CheckCollisionPointRec(mouseWorldPos, exitButtonRect);

        // Check for button clicks
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !isTransitioningOut && !isTransitioningIn)
        {
            if (isStartButtonHovered)
            {
                PlaySoundFx(FX_SELECT);
                transitionCallback = GameUpdate;
                isTransitioningOut = true;
            }
            else if (isOptionsButtonHovered)
            {
                PlaySoundFx(FX_SELECT);
                transitionCallback = OptionsUpdate;
                isTransitioningOut = true;
            }
            else if (isExitButtonHovered)
            {
                ExitApplication();
            }
        }

        // Play sound when hovering over a button, but only once
        if (isStartButtonHovered)
        {
            if (!isHovering || currentHoveredButton != 0)
            {
                PlaySoundFx(FX_HOVER);
                isHovering = true;
            }
            currentHoveredButton = 0;
        }
        else if (isOptionsButtonHovered)
        {
            if (!isHovering || currentHoveredButton != 1)
            {
                PlaySoundFx(FX_HOVER);
                isHovering = true;
            }
            currentHoveredButton = 1;
        }
        else if (isExitButtonHovered)
        {
            if (!isHovering || currentHoveredButton != 2)
            {
                PlaySoundFx(FX_HOVER);
                isHovering = true;
            }
            currentHoveredButton = 2;
        }
        else
        {
            currentHoveredButton = NO_BUTTON;  // Use NO_BUTTON instead of NULL
            isHovering = false;
        }

        // Draw

        BeginDrawing();
        BeginMode2D(*camera);
        ClearBackground(RAYWHITE);

        int imageWidth = backgroundTexture.width;
        int imageHeight = backgroundTexture.height;

        float scaleX = (float)BASE_SCREEN_WIDTH / imageWidth;
        float scaleY = (float)BASE_SCREEN_HEIGHT / imageHeight;

        int imageLogoWidth = logoTexture.width;
        int imageLogoHeight = logoTexture.height;

        float scaleLogoX = (float)BASE_SCREEN_WIDTH / imageLogoWidth / 4;
        float scaleLogoY = (float)BASE_SCREEN_HEIGHT / imageLogoHeight / 4;

        // Draw the background with the scaled dimensions
        // DrawTextureEx(backgroundTexture, (Vector2) { baseX, baseY }, 0.0f, fmax(scaleX, scaleY), WHITE);
        
        DrawDayNightCycle(deltaTime);

        // Draw falling items behind the menu
        DrawMenuFallingItems(deltaTime, true);

        // Draw customer images
        DrawCustomerInMenu(deltaTime);

        DrawTextureEx(backgroundOverlayTexture, (Vector2) { baseX, baseY }, 0.0f, fmax(scaleX, scaleY), WHITE);

        // Draw falling items in front of the menu
        DrawMenuFallingItems(deltaTime, false);

        // If transitioning out, move the background to the left
        if (isTransitioningOut)
        {
            float pixelsToMove = 900.0f;
            float durationInSeconds = 0.6f;
            float transitionSpeed = pixelsToMove / durationInSeconds;

            transitionOffset += transitionSpeed * deltaTime;

            if (transitionOffset >= BASE_SCREEN_WIDTH / 2)
            {
                transitionOffset = 0;
                isTransitioningOut = false;
                transitionCallback(camera);
                break;
            }
        }

        // If transitioning in, move the background to the right
        if (isTransitioningIn)
        {
            float pixelsToMove = 900.0f;
            float durationInSeconds = 0.6f;
            float transitionSpeed = pixelsToMove / durationInSeconds;

            transitionOffset -= transitionSpeed * deltaTime;

            if (transitionOffset <= 0)
            {
                transitionOffset = 0;
                isTransitioningIn = false;
            }
        }


        // Left sidebar white 
        DrawTextureEx(backgroundOverlaySidebarTexture, (Vector2) { baseX - transitionOffset, baseY }, 0.0f, fmax(scaleX, scaleY), WHITE);

        // Logo
        DrawTextureEx(logoTexture, (Vector2) { baseX - transitionOffset, baseY - 50 }, 0.0f, fmax(scaleLogoX, scaleLogoY), WHITE);

        // Draw button labels
        DrawTextEx(meowFont, "Start Game", (Vector2) { (int)(startButtonRect.x + 40), (int)(startButtonRect.y + 15) }, 60, 2, isStartButtonHovered ? MAIN_ORANGE : MAIN_BROWN);
        DrawTextEx(meowFont, "Settings", (Vector2) { (int)(optionsButtonRect.x + 40), (int)(optionsButtonRect.y + 15) }, 60, 2, isOptionsButtonHovered ? MAIN_ORANGE : MAIN_BROWN);
        DrawTextEx(meowFont, "Exit", (Vector2) { (int)(exitButtonRect.x + 40), (int)(exitButtonRect.y + 15) }, 60, 2, isExitButtonHovered ? MAIN_ORANGE : MAIN_BROWN);

        // Draw debug
        if (options->showDebug && debugToolToggles.showObjects)
        {
            DrawRectangleLinesEx(startButtonRect, 1, RED);
            DrawRectangleLinesEx(optionsButtonRect, 1, RED);
            DrawRectangleLinesEx(exitButtonRect, 1, RED);
        }

        DrawOuterWorld();

        // Calculate alpha based on the current time
        if (playFade)
        {
            float alpha = (float)(255.0 * (1.0 - fmin(currentTime / fadeOutDuration, 1.0)));
            if (!isFadeOutDone && alpha != 0)
                DrawTextureEx(splashBackgroundTexture, (Vector2) { baseX, baseY }, 0.0f, fmax(splashBackgroundScaleX, splashBackgroundScaleY), (Color) { 255, 255, 255, alpha });
            else
                isFadeOutDone = true;
        }


        if(options->showDebug)
            DrawDebugOverlay(camera);

        EndMode2D();
        EndDrawing();
    }

    UnloadTexture(backgroundTexture);
    CloseWindow();

    // Execute transition if set
    if (transitionCallback != NULL)
    {
        transitionCallback(camera);
    }
}

void SplashUpdate(Camera2D* camera)
{
#if DEBUG_FASTLOAD
    const double beforeStart = 0;
    double splashDuration = 0;
    const double fadeInDuration = 0;
    const double stayDuration = 0;
    const double fadeOutDuration = 0;
    const double afterEnd = 0;

    LoadGlobalAssets();
    MainMenuUpdate(camera, true);
#else
    const double beforeStart = 1.0;
    double splashDuration = 8.0;
    const double fadeInDuration = 2.0;
    const double stayDuration = 3.0;
    const double fadeOutDuration = 2.0;
    const double afterEnd = 2.0;
#endif

    double startTime = GetTime();
    double currentTime = 0;

    int imageWidth = splashBackgroundTexture.width;
    int imageHeight = splashBackgroundTexture.height;

    float scaleX = (float)BASE_SCREEN_WIDTH / imageWidth;
    float scaleY = (float)BASE_SCREEN_HEIGHT / imageHeight;

    BeginDrawing();
    BeginMode2D(*camera);
    ClearBackground(RAYWHITE);

    if (options->showDebug)
        DrawDebugOverlay(camera);

    EndMode2D();
    EndDrawing();

    while (currentTime < beforeStart)
    {
        WindowUpdate(camera);
        currentTime = GetTime() - startTime;

        BeginDrawing();
        BeginMode2D(*camera);
        float alpha = (float)(255.0 * (1.0 - fmin(currentTime / beforeStart, 1.0)));

        DrawTextureEx(splashBackgroundTexture, (Vector2) { baseX, baseY }, 0.0f, fmax(scaleX, scaleY), WHITE);
        DrawRectangle(baseX, baseY, BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT, (Color) { 255, 255, 255, alpha });

        if (options->showDebug)
            DrawDebugOverlay(camera);

        EndMode2D();
        EndDrawing();
    }

    const Sound systemLoad = LoadSound(ASSETS_PATH"audio/Meow1.mp3");
    PlaySound(systemLoad);

    bool doLoadGlobalAssets = true;

    // Reset time
    startTime = GetTime();

    while (currentTime < splashDuration)
    {
        WindowUpdate(camera);
        currentTime = GetTime() - startTime;

        int alpha = 0;

        // Calculate alpha based on the current time
        if (currentTime < fadeInDuration) {
            // Fading in
            alpha = (int)(255.0 * (currentTime / fadeInDuration));
        }
        else if (currentTime < fadeInDuration + stayDuration) {
            // Fully visible (staying)
            alpha = 255;
            if(doLoadGlobalAssets)
			{
				LoadGlobalAssets();
				doLoadGlobalAssets = false;
			}
        }
        else {
            // Fading out
            alpha = (int)(255.0 * (1.0 - fmin((currentTime - fadeInDuration - stayDuration) / fadeOutDuration, 1.0)));
        }

        // Extends splash screen duration if loading takes too long
        if (currentTime > fadeInDuration + stayDuration && doLoadGlobalAssets)
		{
			splashDuration += 0.1;
		}

        BeginDrawing();
        BeginMode2D(*camera);
        ClearBackground(RAYWHITE);
        DrawTextureEx(splashBackgroundTexture, (Vector2) { baseX, baseY }, 0.0f, fmax(scaleX, scaleY), WHITE);
        DrawTextureEx(splashOverlayTexture, (Vector2) { baseX, baseY }, 0.0f, fmax(scaleX, scaleY), (Color) { 255, 255, 255, alpha });
        
        if (options->showDebug)
            DrawDebugOverlay(camera);

        EndMode2D();
        EndDrawing();
    }

    // Reset time
    startTime = GetTime();


    while (currentTime < afterEnd)
    {
        WindowUpdate(camera);
        currentTime = GetTime() - startTime;

        BeginDrawing();
        BeginMode2D(*camera);
        ClearBackground(RAYWHITE);
        DrawTextureEx(splashBackgroundTexture, (Vector2) { baseX, baseY }, 0.0f, fmax(scaleX, scaleY), WHITE);
        DrawTextureEx(splashOverlayTexture, (Vector2) { baseX, baseY }, 0.0f, fmax(scaleX, scaleY), WHITE);
        EndMode2D();
        EndDrawing();
    }

    // UnloadSound(systemLoad);

    MainMenuUpdate(camera, true);
}

int main(void)
{
    // Initialize random seed once at program start
    srand(time(NULL));
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    SetTraceLogCallback(CustomLogger);
    InitWindow(BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT, "SuperMeowMeow");
    InitAudioDevice();
    // Center of screen
    SetWindowPosition(200, 200);

    Camera2D camera = { 0 };
    camera.target = (Vector2){ 0.0f, 0.0f };
    camera.offset = (Vector2){ BASE_SCREEN_WIDTH / 2.0f, BASE_SCREEN_HEIGHT / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    GameOptions _options;
    _options.difficulty = EASY;

#ifdef IS_WEB
    _options.resolution = (Resolution){ 1920, 1080 };  // Set to 1080p for web
    _options.fullscreen = true;  // Set to fullscreen for web
#else
    _options.resolution = (Resolution){ 1280, 720 };
    _options.fullscreen = false;  // Default to windowed mode
#endif
    _options.targetFps = 120;
    _options.showDebug = DEBUG_SHOW;
    _options.musicEnabled = true;
    _options.soundFxEnabled = true;

    options = &_options;

    SetTargetFPS(options->targetFps);
    SetRuntimeResolution(&camera, options->resolution.x, options->resolution.y);


    meowFont = LoadFontEx(ASSETS_PATH"font/SantJoanDespi-Regular.otf", 256, 0, 250);
    SetTextureFilter(meowFont.texture, TEXTURE_FILTER_ANISOTROPIC_4X);

    logoTexture = LoadTexture(ASSETS_PATH"image/elements/studio_logo.png");
    splashBackgroundTexture = LoadTexture(ASSETS_PATH"image/backgrounds/splash.png");
    splashOverlayTexture = LoadTexture(ASSETS_PATH"image/backgrounds/splash_overlay.png");


    SetExitKey(KEY_NULL);

    SplashUpdate(&camera);
    UnloadGlobalAssets();
    return 0;
}
