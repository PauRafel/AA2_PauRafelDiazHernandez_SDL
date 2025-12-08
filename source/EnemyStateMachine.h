#pragma once
#include "Vector2.h"
#include "Transform.h"
#include <functional>

enum EnemyState
{
    STATE_STAY,         
    STATE_SIMPLE_MOVE,  
    STATE_CIRCLE_MOVE,  
    STATE_CHASE,       
    STATE_WAVE_MOVE,    
    STATE_CUSTOM        
};

class EnemyStateMachine
{
private:
    EnemyState _currentState;
    Transform* _transform;

    Vector2 _moveDirection;
    float _moveSpeed;

    Vector2 _circleCenter;
    float _circleRadius;
    float _circleAngle;
    float _angularSpeed;

    Vector2 _waveStartPosition;
    float _waveAmplitude;
    float _waveFrequency;
    float _waveTime;

    Vector2* _targetPosition; 

    float _stateTimer;
    float _stateDuration;

    std::function<void(float)> _customUpdateCallback;

public:
    EnemyStateMachine(Transform* transform);

    void Update(float dt);

    void SetState(EnemyState newState);
    void SetStateStay(float duration = 0.0f);
    void SetStateSimpleMove(Vector2 direction, float speed);
    void SetStateCircleMove(Vector2 center, float radius, float angularSpeed, float startAngle = 0.0f);
    void SetStateChase(Vector2* targetPosition, float speed);
    void SetStateWaveMove(Vector2 direction, float speed, float amplitude, float frequency);
    void SetStateCustom(std::function<void(float)> updateCallback);

    EnemyState GetCurrentState() const { return _currentState; }
    float GetStateTimer() const { return _stateTimer; }
    bool HasStateExpired() const { return _stateDuration > 0.0f && _stateTimer >= _stateDuration; }

    Vector2 GetCircleCenter() const { return _circleCenter; }
    float GetCircleAngle() const { return _circleAngle; }
    Vector2 GetMoveDirection() const { return _moveDirection; }
};