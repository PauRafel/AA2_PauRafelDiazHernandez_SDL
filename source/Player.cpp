#include "Player.h"
#include "Bullet.h"
#include <iostream>
#include <cmath>

Player::Player(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, std::vector<Bullet*>* bulletsVector)
    : ImageObject(texturePath, sourceOffset, sourceSize)
{
    _bulletsVector = bulletsVector;

    _transform->size = Vector2(64.f, 64.f);
    _transform->position = Vector2(200.f, RM.WINDOW_HEIGHT / 2.0f);
    _transform->scale = Vector2(1.f, 1.f);

    _physics->AddCollider(new AABB(_transform->position, _transform->size));
    _physics->SetLinearDrag(1.5f);

    _hasCannonUpgrade = false;  
    _hasLaserUpgrade = false;

    _shieldTexture = nullptr;
    _shieldAnimationTimer = 0.0f;
    _shieldPulseSpeed = 3.0f;
}

void Player::Update(float dt)
{
    Vector2 direction(0.f, 0.f);
    Vector2 previousPosition = _transform->position;

    if (IM.GetEvent(SDLK_W, HOLD) || IM.GetEvent(SDLK_UP, HOLD))
        direction.y -= 1.0f;

    if (IM.GetEvent(SDLK_S, HOLD) || IM.GetEvent(SDLK_DOWN, HOLD))
        direction.y += 1.0f;

    if (IM.GetEvent(SDLK_A, HOLD) || IM.GetEvent(SDLK_LEFT, HOLD))
        direction.x -= 1.0f;

    if (IM.GetEvent(SDLK_D, HOLD) || IM.GetEvent(SDLK_RIGHT, HOLD))
        direction.x += 1.0f;

    if (direction.x != 0.0f || direction.y != 0.0f)
    {
        direction.Normalize();
        _physics->AddForce(direction * _moveSpeed);
    }

    if (IM.GetEvent(SDLK_SPACE, DOWN))
    {
        Shoot();
    }

    if (_transform->position.x < 0)
        _transform->position.x = 0;
    if (_transform->position.x > RM.WINDOW_WIDTH)
        _transform->position.x = RM.WINDOW_WIDTH;
    if (_transform->position.y < 0)
        _transform->position.y = 0;
    if (_transform->position.y > RM.WINDOW_HEIGHT)
        _transform->position.y = RM.WINDOW_HEIGHT;

    float distanceX = _transform->position.x - previousPosition.x;
    _distanceTraveled += distanceX;

    if (_hasTurrets)
    {
        while (_distanceTraveled >= 100.0f)
        {
            _turretAngle -= 45.0f;
            _distanceTraveled -= 100.0f;
        }
        while (_distanceTraveled <= -100.0f)
        {
            _turretAngle += 45.0f;
            _distanceTraveled += 100.0f;
        }
    }

    if (_isInvulnerable)
    {
        _invulnerabilityTimer += dt;
        _shieldAnimationTimer += dt;

        if (_invulnerabilityTimer >= _invulnerabilityDuration)
        {
            _isInvulnerable = false;
            _invulnerabilityTimer = 0.0f;
            _shieldAnimationTimer = 0.0f;
            std::cout << "Invulnerability ended!" << std::endl;
        }
    }

    Object::Update(dt);
}

void Player::Render()
{
    Object::Render();

    if (_isInvulnerable && _shieldTexture != nullptr)
    {
        float pulseScale = 1.0f + 0.2f * std::abs(std::sin(_shieldAnimationTimer * _shieldPulseSpeed));

        Uint8 alpha = (Uint8)(150 + 105 * std::abs(std::sin(_shieldAnimationTimer * _shieldPulseSpeed * 2.0f)));

        SDL_FRect shieldRect;
        float shieldSize = 80.0f * pulseScale;

        shieldRect.x = _transform->position.x - (shieldSize / 2.0f);
        shieldRect.y = _transform->position.y - (shieldSize / 2.0f);
        shieldRect.w = shieldSize;
        shieldRect.h = shieldSize;

        SDL_FRect shieldSource = { 0.0f, 0.0f, 64.0f, 64.0f };

        SDL_SetTextureAlphaModFloat(_shieldTexture, alpha / 255.0f);

        SDL_RenderTexture(
            RM.GetRenderer(),
            _shieldTexture,
            &shieldSource,
            &shieldRect
        );

        SDL_SetTextureAlphaModFloat(_shieldTexture, 1.0f);
    }
}

void Player::Shoot()
{
    Bullet* mainBullet = new Bullet("resources/bullet.png", Vector2(0.f, 0.f), Vector2(32.f, 32.f));
    mainBullet->GetTransform()->position = _transform->position;
    _bulletsVector->push_back(mainBullet);

    if ((_currentState == STATE_CA1 || _currentState >= STATE_CA2) && _cannonAmmo > 0)
    {
        Bullet* cannonBullet1 = new Bullet("resources/bullet.png", Vector2(0.f, 0.f), Vector2(32.f, 32.f));
        cannonBullet1->GetTransform()->position = _transform->position + Vector2(0.f, -20.f);
        _bulletsVector->push_back(cannonBullet1);

        Bullet* cannonBullet2 = new Bullet("resources/bullet.png", Vector2(0.f, 0.f), Vector2(32.f, 32.f));
        cannonBullet2->GetTransform()->position = _transform->position + Vector2(0.f, 20.f);
        _bulletsVector->push_back(cannonBullet2);

        _cannonAmmo--;
    }

    if ((_currentState == STATE_LA1 || _currentState >= STATE_LA2) && _laserAmmo > 0)
    {
        Bullet* laserBullet = new Bullet("resources/bullet.png", Vector2(0.f, 0.f), Vector2(32.f, 32.f));
        laserBullet->GetTransform()->position = _transform->position + Vector2(10.f, 0.f);
        laserBullet->GetTransform()->scale = Vector2(1.5f, 1.5f); 
        _bulletsVector->push_back(laserBullet);

        _laserAmmo--;
    }

    if (_hasTurrets)
    {
        Bullet* turretBullet = new Bullet("resources/bullet.png", Vector2(0.f, 0.f), Vector2(32.f, 32.f));
        turretBullet->GetTransform()->position = _transform->position;
        _bulletsVector->push_back(turretBullet);
    }
}

void Player::ApplyPowerUp(PowerUpState newState)
{
    switch (newState)
    {
    case STATE_NONE:
        _score += 1000;
        break;
    case STATE_CA1:
        _cannonAmmo = 50;
        _currentState = STATE_CA1;
        _hasCannonUpgrade = true;  
        UpdateSprite();          
        break;
    case STATE_LA1:
        _laserAmmo = 30;
        _currentState = STATE_LA1;
        _hasLaserUpgrade = true;  
        UpdateSprite();          
        break;
    case STATE_SPEED:
        _moveSpeed = _baseSpeed * 2.0f;
        _currentState = STATE_SPEED;
        break;
    case STATE_CA2:
        _cannonAmmo = 100;
        _currentState = STATE_CA2;
        _hasCannonUpgrade = true;   
        UpdateSprite();              
        break;
    case STATE_LA2:
        _laserAmmo = 60;
        _currentState = STATE_LA2;
        _hasLaserUpgrade = true;   
        UpdateSprite();              
        break;
    case STATE_SHIELD:
        _isInvulnerable = true;
        _invulnerabilityTimer = 0.0f;
        _shieldAnimationTimer = 0.0f;
        _currentState = STATE_SHIELD;
        _shieldTexture = RM.GetTexture("resources/shield_effect.png");
        std::cout << "INVULNERABLE for " << _invulnerabilityDuration << " seconds!" << std::endl;
        break;
    case STATE_TURRETS:
        _hasTurrets = true;
        _currentState = STATE_TURRETS;
        UpdateSprite();            
        break;
    case STATE_FULL_SHIELD:
        _shield = 100;
        _currentState = STATE_FULL_SHIELD;
        std::cout << "SHIELD FULLY RESTORED!" << std::endl;
        break;
    }
}

void Player::TakeDamage(int damage)
{
    if (_isInvulnerable)
    {
        std::cout << "Damage blocked! (Invulnerable)" << std::endl;
        return;
    }

    _shield -= damage;
    if (_shield < 0)
        _shield = 0;
}

void Player::UpdateSprite()
{
    std::string newTexture = "resources/player.png";

    if (_hasTurrets && _hasCannonUpgrade && _hasLaserUpgrade)
    {
        newTexture = "resources/player_full.png"; 
    }
    else if (_hasTurrets && _hasCannonUpgrade)
    {
        newTexture = "resources/player_cannon_turrets.png"; 
    }
    else if (_hasTurrets && _hasLaserUpgrade)
    {
        newTexture = "resources/player_laser_turrets.png"; 
    }
    else if (_hasCannonUpgrade && _hasLaserUpgrade)
    {
        newTexture = "resources/player_cannon_laser.png"; 
    }
    else if (_hasTurrets)
    {
        newTexture = "resources/player_turrets.png"; 
    }
    else if (_hasCannonUpgrade)
    {
        newTexture = "resources/player_cannon.png"; 
    }
    else if (_hasLaserUpgrade)
    {
        newTexture = "resources/player_laser.png";
    }

    if (_renderer != nullptr)
    {
        delete _renderer;
    }

    _renderer = new ImageRenderer(
        _transform,
        newTexture,
        Vector2(0.f, 0.f),
        Vector2(64.f, 64.f)
    );

    std::cout << "Player sprite updated to: " << newTexture << std::endl;
}