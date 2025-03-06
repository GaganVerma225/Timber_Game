//  Include important libraries here
#include<sstream>
#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>

// Make ode easies to type with "using namespace"
using namespace sf;

// Function declaration
void updateBranches(int seed);

const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

// Where is the player/branch?
// Left or Right
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];

// This is where our game starts from
int main() 
{
    // Create a videomode object
    VideoMode vm(1920,1080);

    // Create and open a window for the game
    RenderWindow window(vm, "Timber!!!", Style::Fullscreen);

    // Create a texture to hold a graphic on the GPU
    Texture textureBackground;

    // Load a graphic into the texture
    textureBackground.loadFromFile("./../graphics/background.png");

    // Create a sprite
    Sprite spriteBackground;

    // Attach the texture to the sprite
    spriteBackground.setTexture(textureBackground);

    // Set the spriteBackground to cover the screen
    spriteBackground.setPosition(0,0);

    // Make a tree sprite
    Texture textureTree;
    textureTree.loadFromFile("./../graphics/tree.png");
    Sprite spriteTree;
    spriteTree.setTexture(textureTree);
    spriteTree.setPosition(810, 0);

    // Make some extra trees

    // Extra Tree 1
    Sprite spriteTree1;
    spriteTree1.setTexture(textureTree);
    spriteTree1.setPosition(15, 0);
    spriteTree1.scale(Vector2f(0.5f, 1.5f));

    // Extra Tree 2
    Sprite spriteTree2;
    spriteTree2.setTexture(textureTree);
    spriteTree2.setPosition(300, -150);
    spriteTree2.scale(Vector2f(0.5f, 1.0f));

    // Extra Tree 3
    Sprite spriteTree3;
    spriteTree3.setTexture(textureTree);
    spriteTree3.setPosition(1300, -180);
    spriteTree3.scale(Vector2f(0.5f, 1.0f));

    // Extra Tree 4
    Sprite spriteTree4;
    spriteTree4.setTexture(textureTree);
    spriteTree4.setPosition(1500, -220);
    spriteTree4.scale(Vector2f(0.5f, 1.0f));

    // Prepare the bee
    Texture textureBee;
    textureBee.loadFromFile("./../graphics/bee.png");
    Sprite spriteBee;
    spriteBee.setTexture(textureBee);
    spriteBee.setPosition(-300, 800);

    // Is the bee currently moving?
    bool beeActive = false;

    // How fast can the bee fly
    float beeSpeed = 0.0f;

    // make cloud sprites from 1 texture
    Texture textureCloud;

    // Load 1 new texture
    textureCloud.loadFromFile("./../graphics/cloud.png");

    // New sprites with the same texture
    const int NUM_CLOUDS = 6;
    Sprite spriteCloud[NUM_CLOUDS];
    bool cloudActive[NUM_CLOUDS];
    float cloudSpeed[NUM_CLOUDS];

    for(int i = 0; i < NUM_CLOUDS; i++)
    {
        spriteCloud[i].setTexture(textureCloud);

        // Position the clouds on the left of the screen
        // at different heights
        spriteCloud[i].setPosition(-300, i * 50);

        // Are the clouds currently on screen?
        cloudActive[i] = false;

        // How fast is each cloud?
        cloudSpeed[i] = 0.0f;
    }

    // Variables to control time itself
    Clock clock;

    // Time bar
    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;
    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color::Red);
    timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

    Time gameTimeTotal;
    float timeRemaining = 10.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    // Track whether the game is running
    bool paused = true;

    // Draw some text
    int score = 0;

    Text messageText;
    Text scoreText;

    // We need to choose a font
    Font font;
    font.loadFromFile("./../fonts/KOMIKAP_.ttf");

    // Set the font to our message
    messageText.setFont(font);
    scoreText.setFont(font);

    // Assign the actual message
    messageText.setString("Press Enter to start!");
    scoreText.setString("Score = 0");

    // Make it really big
    messageText.setCharacterSize(75);
    scoreText.setCharacterSize(100);

    // Choose a color
    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);

    // Position the text
    FloatRect textRect = messageText.getLocalBounds();

    messageText.setOrigin(textRect.left +
        textRect.width / 2.0f,
        textRect.top +
        textRect.height / 2.0f);

    messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

    scoreText.setPosition(20, 20);

    // Background color for score
    FloatRect scoreBound = scoreText.getGlobalBounds();

    // Create a backgound rectangle
    RectangleShape background;
    background.setSize(Vector2f(scoreBound.width+30, scoreBound.height+30));
    Color backgroundColor(0, 0, 0, 127);
    background.setFillColor(backgroundColor);
    background.setPosition(scoreBound.left - 5, scoreBound.top - 15);

    // Prepare 6 branches
    Texture textureBranch;
    textureBranch.loadFromFile("./../graphics/branch.png");

    // Set the texture for each branch sprite
    for (int i = 0; i < NUM_BRANCHES; i++) 
    {
        branches[i].setTexture(textureBranch);
        branches[i].setPosition(-2000, -2000);

        // Set the sprite's origin to dead centre
        // We can then spin it round without changing its position
        branches[i].setOrigin(220, 20);
    }

    // Prepare the player
    Texture texturePlayer;
    texturePlayer.loadFromFile("./../graphics/player.png");
    Sprite spritePlayer;
    spritePlayer.setTexture(texturePlayer);
    spritePlayer.setPosition(580, 720);

    // The player starts on the left
    side playerSide = side::LEFT;

    // Prepare the gravestone
    Texture textureRIP;
    textureRIP.loadFromFile("./../graphics/rip.png");
    Sprite spriteRIP;
    spriteRIP.setTexture(textureRIP);
    spriteRIP.setPosition(600, 860);

    // Prepare the axe
    Texture textureAxe;
    textureAxe.loadFromFile("./../graphics/axe.png");
    Sprite spriteAxe;
    spriteAxe.setTexture(textureAxe);
    spriteAxe.setPosition(700, 830);

    // Line the axe up with the tree
    const float AXE_POSITION_LEFT = 700;
    const float AXE_POSITION_RIGHT = 1075;

    // Prepare the flying log
    Texture textureLog;
    textureLog.loadFromFile("./../graphics/log.png");
    Sprite spriteLog;
    spriteLog.setTexture(textureLog);
    spriteLog.setPosition(810, 720);

    // Some other useful log related variables
    bool logActive = false;
    float logSpeedX = 1000;
    float logSpeedY = -1500;

    // Control the player input
    bool acceptInput = false;

    // Prepare the sounds
    // The player chopping sound
    SoundBuffer chopBuffer;
    chopBuffer.loadFromFile("./../sound/chop.wav");
    Sound chop;
    chop.setBuffer(chopBuffer);

    // The player has met his end under a branch
    SoundBuffer deathBuffer;
    deathBuffer.loadFromFile("./../sound/death.wav");
    Sound death;
    death.setBuffer(deathBuffer);

    // Out of time
    SoundBuffer ootBuffer;
    ootBuffer.loadFromFile("./../sound/out_of_time.wav");
    Sound outOfTime;
    outOfTime.setBuffer(ootBuffer);

    while(window.isOpen()) 
    {
        /*
        ****************************************
        Handle the players input
        ****************************************
        */

        Event event;

        while (window.pollEvent(event))
        {
            if (event.type == Event::KeyReleased && !paused)
            {
                // Listen for key presses again
                acceptInput = true;

                // hide the axe
                spriteAxe.setPosition(2000,
                    spriteAxe.getPosition().y);
            }
        }

        if(Keyboard::isKeyPressed(Keyboard::Escape)) 
        {
            window.close();
        }

        if(Keyboard::isKeyPressed(Keyboard::Return))
        {
            paused = false;

            // Reset the time and the score
            score = 0;
            timeRemaining = 6;

            // Make all the branches disappear -
            // starting in the second position
            for (int i = 1; i < NUM_BRANCHES; i++)
            {
                branchPositions[i] = side::NONE;
            }

            // Make sure the gravestone is hidden
            spriteRIP.setPosition(675, 2000);

            // Move the player into position
            spritePlayer.setPosition(580, 720);

            acceptInput = true;
        }

        // Wrap the player controls to
        // Make sure we are accepting input
        if (acceptInput)
        {
            // More code here next...

            // First handle pressing the right cursor key
            if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                // Make sure the player is on the right
                playerSide = side::RIGHT;

                score ++;

                // Add to the amount of time remaining
                timeRemaining += (2 / score) + .15;

                spriteAxe.setPosition(AXE_POSITION_RIGHT,
                    spriteAxe.getPosition().y);

                spritePlayer.setPosition(1200, 720);

                // Update the branches
                updateBranches(score);

                // Set the log flying to the left
                spriteLog.setPosition(810, 720);
                logSpeedX = -5000;
                logActive = true;

                acceptInput = false;

                // Play a chop sound
                chop.play();
            }

            // Handle the left cursor key
            if (Keyboard::isKeyPressed(Keyboard::Left))
            {
                // Make sure the player is on the left
                playerSide = side::LEFT;

                score++;

                // Add to the amount of time remaining
                timeRemaining += (2 / score) + .15;

                spriteAxe.setPosition(AXE_POSITION_LEFT,
                    spriteAxe.getPosition().y);

                spritePlayer.setPosition(580, 720);

                // update the branches
                updateBranches(score);

                // set the log flying
                spriteLog.setPosition(810, 720);
                logSpeedX = 5000;
                logActive = true;

                acceptInput = false;

                // Play a chop sound
                chop.play();
            }
        }

        /*
        ****************************************
        Update the scene
        ****************************************
        */

        if(!paused)
        {
            // Measure time
            Time dt = clock.restart();

            // Subtract from the amount of time remaining
            timeRemaining -= dt.asSeconds();

            // size up the time bar
            timeBar.setSize(Vector2f(timeBarWidthPerSecond *
                timeRemaining, timeBarHeight));

            if (timeRemaining<= 0.0f) 
            {
                // Pause the game
                paused = true;

                // Change the message shown to the player
                messageText.setString("Out of time!!");

                //Reposition the text based on its new size
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.left +
                    textRect.width / 2.0f,
                    textRect.top +
                    textRect.height / 2.0f);

                messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

                // Play the out of time sound
                outOfTime.play();
            }

            // Setup the bee
            if (!beeActive)
            {
                // How fast is the bee
                srand((int)time(0));
                beeSpeed = (rand() % 200) + 200;

                // How high is the bee
                srand((int)time(0) * 10);
                float height = (rand() % 500) + 500;
                spriteBee.setPosition(2000, height);
                beeActive = true;
            }
            else
            // Move the bee
            {
                spriteBee.setPosition(
                    spriteBee.getPosition().x -
                        (beeSpeed * dt.asSeconds()),
                    spriteBee.getPosition().y);

                // Has the bee reached the left-hand edge of the screen?
                if (spriteBee.getPosition().x < -100)
                {
                    // Set it up ready to be a whole new bee next frame
                    beeActive = false;
                }
            }

            // Manage the clouds
            for(int i=0;i<NUM_CLOUDS;i++)
            {
                if (!cloudActive[i])
                {
                    // How fast is the cloud
                    srand((int)time(0) * 10 * (i+1));
                    cloudSpeed[i] = (rand() % 200) + 1;

                    // How high is the cloud
                    srand((int)time(0) * 10);
                    float height = (rand() % 50) + i*50;
                    spriteCloud[i].setPosition(-300, height);
                    cloudActive[i] = true;
                }
                else
                {
                    spriteCloud[i].setPosition(
                        spriteCloud[i].getPosition().x +
                            (cloudSpeed[i] * dt.asSeconds()),
                        spriteCloud[i].getPosition().y);

                    // Has the cloud reached the right hand edge of the screen?
                    if (spriteCloud[i].getPosition().x > 1920)
                    {
                        // Set it up ready to be a whole new cloud next frame
                        cloudActive[i] = false;
                    }
                }
            }

            // Update the score text
            std::stringstream ss;
            ss<< "Score = " << score;
            scoreText.setString(ss.str());

            // update the branch sprites
            for (int i = 0; i < NUM_BRANCHES; i++)
            {
                float height = i * 150;

                if (branchPositions[i] == side::LEFT)
                {
                    // Move the sprite to the left side
                    branches[i].setPosition(610, height);

                    // Flip the sprite round the other way
                    branches[i].setRotation(180);
                }
                else if (branchPositions[i] == side::RIGHT)
                {
                    // Move the sprite to the right side
                    branches[i].setPosition(1330, height);

                    // Set the sprite rotation to normal
                    branches[i].setRotation(0);
                }
                else
                {
                    // Hide the branch
                    branches[i].setPosition(3000, height);
                }
            }

             // Handle a flying log
            if (logActive)
            {
                spriteLog.setPosition(
                    spriteLog.getPosition().x +
                        (logSpeedX * dt.asSeconds()),

                    spriteLog.getPosition().y +
                        (logSpeedY * dt.asSeconds()));

                // Has the log reached the right hand edge?
                if (spriteLog.getPosition().x < -100 ||
                    spriteLog.getPosition().x > 2000)
                {
                    // Set it up ready to be a whole new log next frame
                    logActive = false;
                    spriteLog.setPosition(810, 720);
                }
            }

            // has the player been squished by a branch?
            if (branchPositions[5] == playerSide)
            {
                // death
                paused = true;
                acceptInput = false;

                // Draw the gravestone
                if(playerSide == side::LEFT)
                {
                    spriteRIP.setPosition(525, 760);
                }
                else 
                {
                    spriteRIP.setPosition(1255, 760);
                }

                // hide the player
                spritePlayer.setPosition(2000, 660);

                // Change the text of the message
                messageText.setString("SQUISHED!!");

                // Center it on the screen
                FloatRect textRect = messageText.getLocalBounds();

                messageText.setOrigin(textRect.left +
                    textRect.width / 2.0f,
                    textRect.top + textRect.height / 2.0f);

                messageText.setPosition(1920 / 2.0f,
                    1080 / 2.0f);

                // Play the death sound
                death.play();
            }

        } // End if(!paused)

        /*
        ****************************************
        Draw the scene
        ****************************************
        */



        // Clear everything from the last frame
        window.clear();

        // Draw our game scene here
        window.draw(spriteBackground);

        // Draw the clouds
        for(int i = 0; i < NUM_CLOUDS; i++) 
        {
            window.draw(spriteCloud[i]);
        }

        // Draw the extra trees
        window.draw(spriteTree1);
        window.draw(spriteTree2);
        window.draw(spriteTree3);
        window.draw(spriteTree4);

        // Draw the branches
        for (int i = 0; i < NUM_BRANCHES; i++) {
            window.draw(branches[i]);
        }

        // Draw the tree
        window.draw(spriteTree);

        // Draw the player
        window.draw(spritePlayer);

        // Draw the axe
        window.draw(spriteAxe);
        
        // Draw the flying log
        window.draw(spriteLog);
        
        // Draw the gravestone
        window.draw(spriteRIP);
        
        // Draw the insect
        window.draw(spriteBee);

        // Draw the score
        window.draw(background);
        window.draw(scoreText);

        // Draw the timebar
        window.draw(timeBar);

        if (paused)
        {
            // Draw our message
            window.draw(messageText);
        }

        // Show everything we just drew
        window.display();
    }

    return 0;
}

// Function definition
void updateBranches(int seed)
{
    // Move all the branches down one place
    for (int j = NUM_BRANCHES-1; j > 0; j--) {
        branchPositions[j] = branchPositions[j - 1];
    }

    // Spawn a new branch at position 0
    // LEFT, RIGHT or NONE
    srand((int)time(0)+seed);
    int r = (rand() % 5);

    switch (r) {
    case 0:
        branchPositions[0] = side::LEFT;
        break;

    case 1:
        branchPositions[0] = side::RIGHT;
        break;

    default:
        branchPositions[0] = side::NONE;
        break;
    }
}