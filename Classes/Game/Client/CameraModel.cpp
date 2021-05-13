#include "CameraModel.h"

const float CameraModel::DEFAULT_ZOOM_LEVEL = 2.f
;
CameraModel::CameraModel()
: m_position(0,0)
, m_targetPosition(0,0)
, m_zoom(DEFAULT_ZOOM_LEVEL)
, m_targetZoom(DEFAULT_ZOOM_LEVEL)
, m_maxZoom(16.0f)
, m_minZoom(1.0f)
, m_panElasticity(0.95f)
, m_zoomElasticity(0.1f)
, m_maxDistance(200.f)
, m_shakeDampenFactor(0.25f)
, m_elasticPan(false)
, m_elasticZoom(true)
, m_pixelPerfect(false)
, m_enableScreenShake(true)
, m_cameraFollowPlayerID(0)
, m_followPlayer(false)
{
}
