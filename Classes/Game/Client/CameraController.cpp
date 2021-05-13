#include "CameraController.h"
#include "CameraModel.h"
#include "Core/Injector.h"

CameraController::CameraController(std::shared_ptr<CameraModel> model)
: m_model(model)
{
}

CameraController::~CameraController()
{
    m_model = nullptr;
}

void CameraController::initialize()
{
}

void CameraController::update(const float deltaTime)
{
    updatePan(deltaTime);
    updateZoom(deltaTime);
}

cocos2d::Vec2 CameraController::getViewPosition() const
{
    cocos2d::Vec2 midWindow = cocos2d::Director::getInstance()->getWinSize()*0.5;
    return midWindow - (m_model->getPosition() * m_model->getZoom());
}

cocos2d::Rect CameraController::getViewRect() const
{
    cocos2d::Rect viewRect = cocos2d::Rect(-getViewPosition()/m_model->getZoom(), cocos2d::Director::getInstance()->getWinSize()/m_model->getZoom());
    return viewRect;
}

void CameraController::updatePan(const float deltaTime)
{
    const cocos2d::Vec2 targetPosition = m_model->getTargetPosition();
    cocos2d::Vec2 newPosition;
    if (m_model->getElasticPan())
    {
        const float alpha = 1.0f - m_model->getPanElasticity();
        newPosition = m_model->getPosition().lerp(targetPosition, alpha);
    }
    else
    {
        newPosition = targetPosition;
    }
    
    const float maxDistance = m_model->getMaxDistance();
    if (maxDistance > 0.f)
    {
        cocos2d::Vec2 motionVector = targetPosition - m_model->getPosition();
        const float distance = motionVector.length();
        if (distance > maxDistance / m_model->getZoom())
        {
            motionVector.normalize();
            newPosition = targetPosition - (motionVector * (maxDistance / m_model->getZoom()));
        }
    }
    
    if (m_model->getEnableScreenShake())
    {
        const auto& shake = m_model->getScreenShake();
        if (shake.length() < 3.f)
        {
            m_model->setScreenShake(cocos2d::Vec2::ZERO);
        }
        else
        {
            const cocos2d::Vec2 offset = shake * cocos2d::random(-1.f, 1.f);
            newPosition += offset;
            m_model->setScreenShake(shake * (1.f - m_model->getShakeDampenFactor()));
        }
    }
    
    m_model->setPosition(newPosition);
}

void CameraController::updateZoom(const float deltaTime)
{
    float targetZoom = m_model->getTargetZoom();
    if (m_model->getElasticZoom())
    {
        float alpha = m_model->getZoomElasticity();
        float newZoom = (m_model->getZoom() * (1.0f - alpha)) + (targetZoom * alpha);
        m_model->setZoom(newZoom);
        return;
    }
    
    m_model->setZoom(targetZoom);
}
