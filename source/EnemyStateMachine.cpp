#include "EnemyStateMachine.h"
#include <cmath>

EnemyStateMachine::EnemyStateMachine(Transform* transform)
    : _transform(transform), _currentState(STATE_STAY)
{
    _moveDirection = Vector2(0.f, 0.f);
    _moveSpeed = 0.f;
    _circleCenter = Vector2(0.f, 0.f);
    _circleRadius = 0.f;
    _circleAngle = 0.f;
    _angularSpeed = 0.f;
    _waveStartPosition = Vector2(0.f, 0.f);
    _waveAmplitude = 0.f;
    _waveFrequency = 0.f;
    _waveTime = 0.f;
    _targetPosition = nullptr;
    _stateTimer = 0.f;
    _stateDuration = 0.f;
    _customUpdateCallback = nullptr;
}

void EnemyStateMachine::Update(float dt)
{
    _stateTimer += dt;

    switch (_currentState)
    {
    case STATE_STAY:
        break;

    case STATE_SIMPLE_MOVE:
        _transform->position = _transform->position + _moveDirection * _moveSpeed * dt;
        break;

    case STATE_CIRCLE_MOVE:
        _circleAngle += _angularSpeed * dt;
        _transform->position = Vector2(
            _circleCenter.x + _circleRadius * cos(_circleAngle),
            _circleCenter.y + _circleRadius * sin(_circleAngle)
        );
        break;

    case STATE_CHASE:
    {
        if (_targetPosition != nullptr)
        {
            Vector2 direction = *_targetPosition - _transform->position;
            if (direction.Magnitude() > 0.01f)
            {
                direction.Normalize();
                _transform->position = _transform->position + direction * _moveSpeed * dt;
            }
        }
        break;
    }

    case STATE_WAVE_MOVE:
    {
        _waveTime += dt;
        _transform->position = _transform->position + _moveDirection * _moveSpeed * dt;

        Vector2 perpendicular(-_moveDirection.y, _moveDirection.x);
        float offset = _waveAmplitude * sin(_waveFrequency * _waveTime);
        _transform->position = _transform->position + perpendicular * offset * dt * 10.0f;
        break;
    }

    case STATE_CUSTOM:
        if (_customUpdateCallback != nullptr)
            _customUpdateCallback(dt);
        break;
    }
}

void EnemyStateMachine::SetState(EnemyState newState)
{
    _currentState = newState;
    _stateTimer = 0.f;
    _stateDuration = 0.f;
}

void EnemyStateMachine::SetStateStay(float duration)
{
    _currentState = STATE_STAY;
    _stateTimer = 0.f;
    _stateDuration = duration;
}

void EnemyStateMachine::SetStateSimpleMove(Vector2 direction, float speed)
{
    _currentState = STATE_SIMPLE_MOVE;
    _moveDirection = direction;
    _moveDirection.Normalize();
    _moveSpeed = speed;
    _stateTimer = 0.f;
    _stateDuration = 0.f;
}

void EnemyStateMachine::SetStateCircleMove(Vector2 center, float radius, float angularSpeed, float startAngle)
{
    _currentState = STATE_CIRCLE_MOVE;
    _circleCenter = center;
    _circleRadius = radius;
    _angularSpeed = angularSpeed;
    _circleAngle = startAngle;
    _stateTimer = 0.f;
    _stateDuration = 0.f;

    _transform->position = Vector2(
        _circleCenter.x + _circleRadius * cos(_circleAngle),
        _circleCenter.y + _circleRadius * sin(_circleAngle)
    );
}

void EnemyStateMachine::SetStateChase(Vector2* targetPosition, float speed)
{
    _currentState = STATE_CHASE;
    _targetPosition = targetPosition;
    _moveSpeed = speed;
    _stateTimer = 0.f;
    _stateDuration = 0.f;
}

void EnemyStateMachine::SetStateWaveMove(Vector2 direction, float speed, float amplitude, float frequency)
{
    _currentState = STATE_WAVE_MOVE;
    _moveDirection = direction;
    _moveDirection.Normalize();
    _moveSpeed = speed;
    _waveAmplitude = amplitude;
    _waveFrequency = frequency;
    _waveTime = 0.f;
    _waveStartPosition = _transform->position;
    _stateTimer = 0.f;
    _stateDuration = 0.f;
}

void EnemyStateMachine::SetStateCustom(std::function<void(float)> updateCallback)
{
    _currentState = STATE_CUSTOM;
    _customUpdateCallback = updateCallback;
    _stateTimer = 0.f;
    _stateDuration = 0.f;
}