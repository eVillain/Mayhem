#include "GameViewController.h"

#include "AudioController.h"
#include "AddLightEvent.h"
#include "CameraController.h"
#include "CameraModel.h"
#include "CollisionUtils.h"
#include "CrosshairView.h"
#include "Core/Dispatcher.h"
#include "EntityDataModel.h"
#include "EntityView.h"
#include "GameView.h"
#include "GameViewConstants.h"
#include "HUDView.h"
#include "Core/Injector.h"
#include "Game/Client/InputModel.h"
#include "LightController.h"
#include "ParticlesController.h"
#include "Pseudo3DItem.h"
#include "Pseudo3DParticle.h"
#include "Pseudo3DSprite.h"
#include "SharedConstants.h"
#include "SpawnParticlesEvent.h"
#include "WeaponConstants.h"
#include "GameSettings.h"
#include "LevelModel.h"
#include "RaycastUtil.h"
#include "base/ccUtils.h"
#include "SnapshotModel.h"
#include "InputConstants.h"

GameViewController::GameViewController(std::shared_ptr<GameSettings> gameSettings,
                                       std::shared_ptr<LevelModel> levelModel,
                                       std::shared_ptr<GameView> gameView,
                                       std::shared_ptr<InputModel> inputModel,
                                       std::shared_ptr<LightController> lightController,
                                       std::shared_ptr<ParticlesController> particlesController,
                                       std::shared_ptr<HUDView> hudView,
                                       std::shared_ptr<AudioController> audioController)
: m_gameSettings(gameSettings)
, m_cameraController(nullptr)
, m_cameraModel(nullptr)
, m_levelModel(levelModel)
, m_gameView(gameView)
, m_inputModel(inputModel)
, m_lightController(lightController)
, m_particlesController(particlesController)
, m_hudView(hudView)
, m_audioController(audioController)
, m_postProcessShader(nullptr)
, m_postProcessNoLightShader(nullptr)
, m_shotHitLastFrame(false)
{
    m_cameraModel = std::make_shared<CameraModel>();
    m_cameraController = std::make_shared<CameraController>(m_cameraModel);
    
    m_postProcessShader = cocos2d::GLProgram::createWithFilenames("res/shaders/vertex_p.vsh", "res/shaders/post_process.fsh");
    m_postProcessNoLightShader = cocos2d::GLProgram::createWithFilenames("res/shaders/vertex_p.vsh", "res/shaders/post_process_nolight.fsh");
    printf("GameViewController:: constructor: %p\n", this);
}

GameViewController::~GameViewController()
{
    printf("GameViewController:: destructor: %p\n", this);
}

void GameViewController::update(const float deltaTime,
                                const SnapshotData& snapshot,
                                const bool isNewSnapshot,
                                const bool skipLocalPlayerShots)
{
    m_gameView->getPixelDrawNode()->clear();
    m_gameView->getDebugDrawNode()->clear();
    
    if (isNewSnapshot)
    {
        renderHitData(snapshot, skipLocalPlayerShots);
    }
    
    updateEntities(snapshot);
    
    updateCamera(deltaTime, snapshot);

    updateCursor(snapshot);
    
    updateShotTrails(deltaTime);

    updateView(deltaTime);

    m_particlesController->update();
    
    const cocos2d::Value& postProcessSetting = m_gameSettings->getValue(GameViewConstants::SETTING_RENDER_POSTPROCESS, cocos2d::Value(true));
    if (postProcessSetting.asBool())
    {
        renderPostProcess(snapshot);
    }
    
    // DEBUG DRAW SHIZZLE
    if (m_gameView->getDebugDrawNode()->isVisible())
    {
        renderDebug();
    }
}

void GameViewController::onEntityDestroyed(const uint32_t entityID,
                                           const SnapshotData& snapshot)
{
    auto entityViewIt = m_entityViews.find(entityID);
    if (entityViewIt == m_entityViews.end())
    {
        printf("GameViewController::onEntityDestroyed fail: no entity view %u to remove!", entityID);
        return;
    }
    
    auto entityView = entityViewIt->second;
    const EntityType type = entityView->getEntityType();
    const cocos2d::Vec2 position = entityView->getSprite()->getPosition();
    
    if (type == EntityType::Projectile_Grenade ||
        type == EntityType::Projectile_Rocket)
    {
        const float PSEUDO_3D_BLAST_STRENGTH = 64.f;
        m_gameView->drawExplosion(position);

        Dispatcher::globalDispatcher().dispatch(AddLightEvent({position, EXPLOSION_RADIUS, cocos2d::Color4F::WHITE, 0.06f}));
        const StaticEntityData& itemData = EntityDataModel::getStaticEntityData((EntityType)type);
        m_audioController->playPositionalAudio(itemData.weapon.sound, position);

        // Shake up previous pseudo-3D particles
        std::vector<std::shared_ptr<Pseudo3DItem>>& pseudo3DItems = m_gameView->getPseudo3DItems();
        for (auto item : pseudo3DItems)
        {
            const cocos2d::Vec2 distVec = item->getPosition() - position;
            const float dist = distVec.length();
            if (dist < EXPLOSION_RADIUS * 2.f)
            {
                const float distRatio = 1.f - (dist / (EXPLOSION_RADIUS * 2.f));
                const cocos2d::Vec2 newVelocity = item->getVelocity() + (distVec.getNormalized() * (distRatio * PSEUDO_3D_BLAST_STRENGTH));
                const float newZVelocity = (distRatio * PSEUDO_3D_BLAST_STRENGTH);
                
                item->setVelocity(newVelocity);
                item->setPseudoZVelocity(newZVelocity);
                item->setPseudoZPosition(item->getPseudoZPosition() + 8.f * distRatio);

            }
        }
        // Create new explosion fragment particles
        for (int i = 0; i < 32; i++)
        {
            float randomX = cocos2d::random(-3.f, 3.f);
            float randomY = cocos2d::random(-3.f, 3.f);
            float randomZ = cocos2d::random(-3.f, 3.f);
            const cocos2d::Vec2 randomPos = cocos2d::Vec2(randomX, randomY);
            const cocos2d::Vec2 rockParticlePos = position + randomPos;
            const cocos2d::Vec2 distVec = rockParticlePos - position;
            const float dist = distVec.length();
            const float distRatio = 1.f - (dist / EXPLOSION_RADIUS);
            const cocos2d::Vec2 newVelocity = (distVec.getNormalized() * (distRatio * PSEUDO_3D_BLAST_STRENGTH));
            const float newZVelocity = (distRatio * PSEUDO_3D_BLAST_STRENGTH);
            m_gameView->createPseudo3DParticle(cocos2d::Color4F::WHITE,
                                               rockParticlePos,
                                               2.f * (randomZ + 2.f),
                                               newVelocity,
                                               newZVelocity,
                                               20.f,
                                               0.3f);
        }
        
        const auto playerIt = snapshot.playerData.find(m_cameraModel->getCameraFollowPlayerID());
        if (playerIt != snapshot.playerData.end())
        {
            const PlayerState& playerState = playerIt->second;
            const EntitySnapshot& entitySnapshot = snapshot.entityData.at(playerState.entityID);
            const cocos2d::Vec2 entityPosition = cocos2d::Vec2(entitySnapshot.positionX, entitySnapshot.positionY);
            const float playerToExplosion = (entityPosition - position).length();
            if (playerToExplosion < EXPLOSION_RADIUS)
            {
                const float impact = (playerToExplosion / EXPLOSION_RADIUS) * 50.f;
                m_cameraModel->setScreenShake(cocos2d::Vec2(impact, impact));
            }
        }
    }
    else if (type == EntityType::Projectile_Smoke)
    {
        Dispatcher::globalDispatcher().dispatch(SpawnParticlesEvent(ParticleConstants::SMOKE_GRENADE,
                                                                    position,
                                                                    0.f,
                                                                    position));
    }
    
    entityView->getSprite()->stopAllActions();
    m_gameView->getSpriteBatch()->removeChild(entityView->getSprite(), true);
    m_entityViews.erase(entityID);
}

void GameViewController::setCameraFollowPlayerID(const uint8_t playerID)
{
    m_cameraModel->setCameraFollowPlayerID(playerID);
    m_cameraModel->setFollowPlayer(true);
}

void GameViewController::setCameraFree()
{
    m_cameraModel->setCameraFollowPlayerID(0);
    m_cameraModel->setFollowPlayer(false);
}

void GameViewController::onEntitySpawned(const uint32_t entityID,
                                         const EntitySnapshot& data)
{
    std::shared_ptr<EntityView> entityView;
    const cocos2d::Vec2 position = cocos2d::Vec2(data.positionX,
                                                 data.positionY);
    if (data.type == EntityType::PlayerEntity)
    {
        entityView = std::make_shared<EntityView>(entityID, (EntityType)data.type);
        entityView->setupAnimations(CharacterType::Character_Base);
        m_gameView->getSpriteBatch()->addChild(entityView->getSprite(), GameViewConstants::Z_ORDER_PLAYER);
    }
    else if ((data.type > EntityType::Item_First_Placeholder &&
             data.type < EntityType::Item_Last_Placeholder) ||
             data.type == EntityType::Loot_Box)
    {
        entityView = std::make_shared<EntityView>(entityID, (EntityType)data.type);
        const StaticEntityData& itemData = EntityDataModel::getStaticEntityData((EntityType)data.type);
        entityView->setupSprite(itemData.sprite);
        entityView->getSprite()->setAnchorPoint(itemData.anchor);
        m_gameView->getSpriteBatch()->addChild(entityView->getSprite(), GameViewConstants::Z_ORDER_ITEM);
    }
    else if (data.type >= EntityType::Projectile_Bullet &&
             data.type <= EntityType::Projectile_Smoke)
    {
        entityView = std::make_shared<EntityView>(entityID, (EntityType)data.type);
        entityView->setupSprite(EntityDataModel::getStaticEntityData((EntityType)data.type).sprite);
        m_gameView->getSpriteBatch()->addChild(entityView->getSprite(), GameViewConstants::Z_ORDER_PROJECTILE);

        if (data.type == EntityType::Projectile_Rocket)
        {
            // Add fire and smoke trails to rocket
            const cocos2d::Vec2 smokeOffset = cocos2d::Vec2::forAngle(data.rotation) * 20.f;
            const cocos2d::Vec2 flameOffset = cocos2d::Vec2::forAngle(data.rotation) * 8.f;
            Dispatcher::globalDispatcher().dispatch(SpawnParticlesAttachedEvent(ParticleConstants::SMOKE_TRAIL,
                                                                                entityView->getSprite(),
                                                                                180.f + data.rotation * (180.f / M_PI),
                                                                                cocos2d::Vec2(-smokeOffset.x, smokeOffset.y),
                                                                                cocos2d::ParticleSystem::PositionType::FREE));
            Dispatcher::globalDispatcher().dispatch(SpawnParticlesAttachedEvent(ParticleConstants::FIRE,
                                                                                entityView->getSprite(),
                                                                                180.f + data.rotation * (180.f / M_PI),
                                                                                cocos2d::Vec2(-flameOffset.x, flameOffset.y),
                                                                                cocos2d::ParticleSystem::PositionType::GROUPED,
                                                                                true));

        }
    }
    
    if (entityView)
    {
        // TODO: Sort Z-Order every frame
        const cocos2d::Size mapSize = m_levelModel->getMapSize();
        entityView->getSprite()->setPosition(position);
        entityView->getSprite()->setLocalZOrder(mapSize.height - position.y);
        entityView->getSprite()->setRotation(-data.rotation * (180.f / M_PI));
        m_entityViews[entityID] = entityView;
    }
    else
    {
        // Unknown entity type, plz implement now!
        assert(false);
    }
}

void GameViewController::renderShot(const uint32_t shooterEntityID,
                                    const uint8_t shooterPlayerID,
                                    const cocos2d::Vec2 shooterPosition,
                                    const bool shooterFlipX,
                                    const cocos2d::Vec2 aimPoint,
                                    const EntityType weaponType,
                                    const SnapshotData& snapshot)
{
    if (!m_entityViews.count(shooterEntityID))
    {
        printf("GameViewController::renderShot fail: entity view for shooter entity: %i!\n", shooterEntityID);
        return;
    }

    const auto& itemData = EntityDataModel::getStaticEntityData(weaponType);
    const WeaponConstants::WeaponStateData data = WeaponConstants::getWeaponData(shooterPosition,
                                                                                 aimPoint,
                                                                                 itemData.weapon.holdOffset,
                                                                                 itemData.weapon.projectileOffset,
                                                                                 shooterFlipX);

    const float particleDegrees = data.weaponAngle + (shooterFlipX ? 180.f : 0.f);
    // Show effects and play sound
    if (itemData.weapon.damageType == Damage_Type_Hitscan ||
        itemData.weapon.damageType == Damage_Type_Projectile)
    {
        const float RAY_LENGTH = itemData.weapon.type == WeaponType::Weapon_Type_Shotgun ? 100.f : 500.f;
        const cocos2d::Vec2 ray = (aimPoint - data.projectilePosition).getNormalized();
        const cocos2d::Vec2 shotEnd = data.projectilePosition + ray * RAY_LENGTH;
        
        if (itemData.weapon.type == WeaponType::Weapon_Type_Shotgun)
        {
            const cocos2d::Vec2 shotEnd = data.projectilePosition + (aimPoint - data.projectilePosition).getNormalized() * RAY_LENGTH;
            const cocos2d::Vec2 offset = (data.projectilePosition - shotEnd).getNormalized().getPerp();

            for (int shot = -4; shot < 4; shot++)
            {
                const cocos2d::Vec2 shotOffset = offset * shot;
                renderHitEffects(data.projectilePosition, shotEnd + shotOffset, weaponType, data, shooterEntityID, snapshot.entityData);
            }
        }
        else if (itemData.weapon.damageType == Damage_Type_Hitscan)
        {
            renderHitEffects(data.projectilePosition, shotEnd, weaponType, data, shooterEntityID, snapshot.entityData);
        }
        
        auto weaponSprite = m_entityViews.at(shooterEntityID)->getSecondarySprites()[EntityView::WEAPON_INDEX];
        if (!weaponSprite)
        {
            assert(false); // Why is there no weapon sprite? This needs to be caught and fixed (seems to occur in replays)
        }
        Dispatcher::globalDispatcher().dispatch(AddLightEvent({data.projectilePosition, 64.f, cocos2d::Color4F::WHITE, 0.06f}));
        Dispatcher::globalDispatcher().dispatch(SpawnParticlesAttachedEvent(ParticleConstants::MUZZLE_FLASH,
                                                                            weaponSprite,
                                                                            particleDegrees,
                                                                            data.projectilePosition - weaponSprite->getPosition(),
                                                                            cocos2d::ParticleSystem::PositionType::GROUPED,
                                                                            true));
        // Bullet shell
        m_gameView->createPseudo3DParticle(cocos2d::Color4F::YELLOW,
                                           shooterPosition - cocos2d::Vec2(0, 8),
                                           8.f,
                                           cocos2d::Vec2(0, -8),
                                           32.f,
                                           3.f,
                                           0.3f);
        m_audioController->playPositionalAudio(itemData.weapon.sound, data.projectilePosition);
    }
    
    if (shooterPlayerID == m_cameraModel->getCameraFollowPlayerID())
    {
        // Apply recoil
        const float recoilAmount = cocos2d::random(3.f, 6.f);
        const cocos2d::Vec2 ray = (aimPoint - data.projectilePosition).getNormalized();
        const cocos2d::Vec2 recoil = ray.getPerp() * (shooterFlipX ? -recoilAmount : recoilAmount);
        auto glview = cocos2d::Director::getInstance()->getOpenGLView();
        const cocos2d::Vec2 cursorPos = glview->getCursorPosition();
        m_inputModel->setMouseCoord(m_inputModel->getMouseCoord() + recoil);

//        // Show screen shake
//        m_cameraModel->setScreenShake(cocos2d::Vec2(5.f, 5.f));
    }
}

void GameViewController::renderHitEffects(const cocos2d::Vec2& shotStart,
                                          const cocos2d::Vec2& shotEnd,
                                          const EntityType weaponType,
                                          const WeaponConstants::WeaponStateData& data,
                                          const uint16_t entityID,
                                          const std::map<uint32_t, EntitySnapshot>& entityData)
{
    RayElement raycastResult = RaycastUtil::rayCast2(shotStart,
                                                     shotEnd,
                                                     entityID,
                                                     entityData,
                                                     m_levelModel->getStaticRects());
    const cocos2d::Vec2 fullRay = raycastResult.shotEndPoint - raycastResult.shotStartPoint;
    const cocos2d::Vec2 ray = fullRay.getNormalized();

    if (raycastResult.hitEntityID != 0)
    {
        const auto& hitEntity = entityData.at(raycastResult.hitEntityID);
        if (hitEntity.type == PlayerEntity)
        {
            // Cast ray to other side of player to find blood spatter
            RayElement raycastResult2 = RaycastUtil::rayCast2(raycastResult.shotEndPoint + ray * 20.0f,
                                                              raycastResult.shotEndPoint,
                                                              entityID,
                                                              entityData,
                                                              m_levelModel->getStaticRects());
            for (int i = 0; i < 8; i++)
            {
                const float randomX = cocos2d::random(-1.f, 1.f);
                const float randomY = cocos2d::random(-1.f, 1.f);

                cocos2d::Vec2 bloodSpatterDirection = ray;
                bloodSpatterDirection.x += randomX * 0.1f;
                bloodSpatterDirection.y += randomX * 0.1f;
                bloodSpatterDirection.normalize();
                const float yOffset = 8.f + ((randomY + 1.f) * 2.f);
                m_gameView->createPseudo3DParticle(cocos2d::Color4F::RED,
                                                   raycastResult2.shotEndPoint - cocos2d::Vec2(0, yOffset),
                                                   yOffset,
                                                   bloodSpatterDirection * 32.f,
                                                   8.f * (randomY + 1.f),
                                                   10.f,
                                                   0.f);
            }
        }
        else
        {
            Dispatcher::globalDispatcher().dispatch(SpawnParticlesEvent(ParticleConstants::SPARKS,
                                                                        raycastResult.shotEndPoint,
                                                                        0.f,
                                                                        cocos2d::Vec2::ZERO,
                                                                        true));
            const float debrisRandom = cocos2d::random(-1.f, 1.f);
            cocos2d::Vec2 debrisDirection = (raycastResult.shotEndPoint - raycastResult.shotStartPoint);
            debrisDirection.x *= debrisRandom;
            debrisDirection.normalize();
            m_gameView->createPseudo3DParticle(cocos2d::Color4F::YELLOW, raycastResult.shotEndPoint, 2.f, -debrisDirection * 64.f, 64.f, 10.f, 0.3f);
        }
    }
    else if (shotEnd != raycastResult.shotEndPoint)
    {
        Dispatcher::globalDispatcher().dispatch(SpawnParticlesEvent(ParticleConstants::DUST_SMALL,
                                                                    raycastResult.shotEndPoint,
                                                                    0.f));
        const float debrisRandom = cocos2d::random(-1.f, 1.f);
        cocos2d::Vec2 debrisDirection = (raycastResult.shotEndPoint - raycastResult.shotStartPoint);
        debrisDirection.x *= debrisRandom;
        debrisDirection.normalize();
        m_gameView->createPseudo3DParticle(cocos2d::Color4F::GRAY, raycastResult.shotEndPoint, 2.f, -debrisDirection * 64.f, 64.f, 10.f, 0.2f);
    }

    // Show immediate trail of bullet
    m_shotTrails.push_back({1.f, raycastResult.shotStartPoint, raycastResult.shotEndPoint});

    // Smoke trails and railgun rails
    const float segmentLength = 50.f;
    const int segments = (fullRay.length() / segmentLength) + 1;
    for (int i = 0; i < segments; i++)
    {
        const cocos2d::Vec2 start = raycastResult.shotStartPoint + (ray * (segmentLength * i));
        const cocos2d::Vec2 end = i == segments - 1 ? raycastResult.shotEndPoint : raycastResult.shotStartPoint + (ray * (segmentLength * (i + 1)));

        if (weaponType == EntityType::Item_Railgun)
        {
            Dispatcher::globalDispatcher().dispatch(SpawnParticlesEvent(ParticleConstants::RAILGUN_RAIL,
                                                                        start,
                                                                        -ray.getAngle() * (180.f / M_PI),
                                                                        end));
        }
        else
        {
            Dispatcher::globalDispatcher().dispatch(SpawnParticlesEvent(ParticleConstants::BULLET_SMOKE_TRAIL,
                                                                        start,
                                                                        -ray.getAngle() * (180.f / M_PI),
                                                                        end));
        }
    }
}

void GameViewController::renderHitData(const SnapshotData& snapshot,
                                       const bool skipLocalPlayerShots)
{
    const std::vector<FrameHitData>& hitData = snapshot.hitData;
    for (const auto& hit : hitData)
    {
        const cocos2d::Vec2 hitPos = cocos2d::Vec2(hit.hitPosX, hit.hitPosY);
        cocos2d::Vec2 hitterPos = hitPos;
        cocos2d::Vec2 hitRay;
        const auto& hitterEntityIt = snapshot.entityData.find(hit.hitterEntityID);
        if (hitterEntityIt != snapshot.entityData.end())
        {
            hitterPos = cocos2d::Vec2(hitterEntityIt->second.positionX, hitterEntityIt->second.positionY);
            hitRay = (hitPos - hitterPos).getNormalized();
        }
        
        if (hit.damage > 0)
        {
            m_hudView->showHealthBlimp(-hit.damage, m_gameView->toViewPosition(hitPos + cocos2d::Vec2(0.f, 20.f)));
            const float hitAngle = hitRay.getAngle();
            // Damage taken by some entity - Show blood particles
            Dispatcher::globalDispatcher().dispatch(SpawnParticlesEvent(ParticleConstants::BLOOD_WOUND,
                                                                        hitPos,
                                                                        hitAngle));
            if (hit.isLethal)
            {
                auto entityViewIt = m_entityViews.find(hit.hitEntityID);
                if (entityViewIt != m_entityViews.end())
                {
                    // Player died from this hit, show appropriate gore effects
                    renderPlayerDeath(entityViewIt->second->getSprite()->getPosition(), hitRay, hit.isHeadshot);
                }
            }
            else
            {
                auto hitPlayerIt = std::find_if(snapshot.playerData.begin(),
                                                snapshot.playerData.end(),
                                                [&hit](const std::pair<uint8_t, PlayerState>& pair) {
                    return pair.second.entityID == hit.hitEntityID;
                });
                if (hitPlayerIt != snapshot.playerData.end() &&
                    hitPlayerIt->second.health > 0.f)
                {
                    // Player took a hit but didn't die, just show some extra blood at hit position
                    Dispatcher::globalDispatcher().dispatch(SpawnParticlesEvent(ParticleConstants::BLOOD_SPLASH,
                                                                                cocos2d::Vec2(hit.hitPosX, hit.hitPosY),
                                                                                0.f));
                }
            }
        }

        auto hitterPlayerIt = std::find_if(snapshot.playerData.begin(),
                                           snapshot.playerData.end(),
                                           [&hit](const std::pair<uint8_t, PlayerState>& pair) {
            return pair.second.entityID == hit.hitterEntityID;
        });
        if (hitterPlayerIt == snapshot.playerData.end())
        {
            printf("GameViewController::renderHitData fail: no hitter player data in snapshot!");
            continue;
        }
        
        const uint8_t localPlayerID = m_cameraModel->getCameraFollowPlayerID();
        if (skipLocalPlayerShots &&
            hitterPlayerIt->first == localPlayerID)
        {
            continue; // Locally predicted shots were already rendered so skip them
        }
        
        if (hitterEntityIt == snapshot.entityData.end())
        {
            printf("GameViewController::renderHitData fail: no hitter entity data in snapshot!");
            continue;
        }
        
        const auto& hitterPlayer = hitterPlayerIt->second;
        const InventoryItemState& weapon = hitterPlayer.weaponSlots.at(hitterPlayer.activeWeaponSlot);
        if (weapon.type == EntityType::PlayerEntity)
        {
            printf("GameViewController::renderHitData fail: hitter has no weapon in active weapon slot!");
            continue;
        }
        
        renderShot(hitterPlayer.entityID,
                   hitterPlayerIt->first,
                   hitterPos,
                   hitterPlayer.flipX,
                   cocos2d::Vec2(hit.hitPosX, hit.hitPosY),
                   (EntityType)weapon.type,
                   snapshot);
    }
}

void GameViewController::updateCamera(const float deltaTime,
                                      const SnapshotData& snapshot)
{
    if (m_cameraModel->getFollowPlayer())
    {
        const auto playerIt = snapshot.playerData.find(m_cameraModel->getCameraFollowPlayerID());
        if (playerIt == snapshot.playerData.end())
        {
            return;
        }
        const uint16_t localPlayerEntityID = playerIt->second.entityID;
        // Alternative method to pan camera ahead to the predicted position
    //    if (snapshot.entityData.count(localPlayerEntityID))
    //    {
    //        const auto& player = snapshot.entityData.at(localPlayerEntityID);
    //        m_cameraModel->setTargetPosition(cocos2d::Vec2(player.positionX, player.positionY));
    //    }
        if (m_entityViews.count(localPlayerEntityID))
        {
            auto playerView = m_entityViews.at(localPlayerEntityID);
            m_cameraModel->setTargetPosition(playerView->getSprite()->getPosition());
        }
    }
    else
    {
        const cocos2d::Vec2 inputDir = cocos2d::Vec2(m_inputModel->getInputValue(InputConstants::ACTION_MOVE_RIGHT),
                                                     -m_inputModel->getInputValue(InputConstants::ACTION_MOVE_UP));
        const cocos2d::Vec2 targetPosition = m_cameraModel->getTargetPosition() + cocos2d::Vec2(inputDir.x, -inputDir.y) * 4.f;
        m_cameraModel->setTargetPosition(targetPosition);
    }

    float zoom = m_cameraModel->getTargetZoom();
    if (m_inputModel->getInputValue(InputConstants::ACTION_ZOOM_IN))
    {
        zoom *= 1.05f;
    }
    if (m_inputModel->getInputValue(InputConstants::ACTION_ZOOM_OUT))
    {
        zoom *= 0.95f;
    }

    zoom = std::max(std::min(zoom, m_cameraModel->getMaxZoom()), m_cameraModel->getMinZoom());

    m_cameraModel->setTargetZoom(zoom);
    m_cameraController->update(deltaTime);
}

void GameViewController::updateView(const float deltaTime)
{
    std::vector<std::shared_ptr<Pseudo3DItem>>& pseudo3DItems = m_gameView->getPseudo3DItems();
    for (auto item : pseudo3DItems)
    {
        item->update(deltaTime);
        if (auto particle = std::dynamic_pointer_cast<Pseudo3DParticle>(item))
        {
            m_gameView->getPixelDrawNode()->drawPoint(particle->getScreenPosition(), 1.f, particle->getColor());
            if (particle->getPseudoZPosition() > 0.f) // Shadow
            {
                m_gameView->getPixelDrawNode()->drawPoint(particle->getPosition(), 1.f, cocos2d::Color4F(0.f,0.f,0.f,0.5f));
            }
        }
        else if (auto sprite = std::dynamic_pointer_cast<Pseudo3DSprite>(item))
        {
            if (sprite->getTimeAlive() >= sprite->getLifeTime())
            {
                sprite->getSprite()->removeFromParent();
            }
        }
    }
    
    pseudo3DItems.erase(std::remove_if(pseudo3DItems.begin(),
                                       pseudo3DItems.end(),
                                       [](const std::shared_ptr<Pseudo3DItem>& item)
                                       {
                                           return item->getTimeAlive() >= item->getLifeTime();
                                       }),
                        pseudo3DItems.end());
    
    auto director = cocos2d::Director::getInstance();
    const cocos2d::Vec2 midWindow = director->getWinSize() * 0.5f;
    const float zoom = m_cameraModel->getZoom();
    const cocos2d::Value& deferredRenderSetting = m_gameSettings->getValue(GameViewConstants::SETTING_RENDER_DEFERRED, cocos2d::Value(true));
    if (deferredRenderSetting.asBool())
    {
        cocos2d::Vec2 viewPosition = (midWindow * director->getContentScaleFactor()) - m_cameraModel->getPosition();
        if (m_cameraModel->getPixelPerfet())
        {
            viewPosition = cocos2d::Vec2(std::floor(viewPosition.x),
                                         std::floor(viewPosition.y));
        }
        m_gameView->getView()->setPosition(viewPosition);
        m_gameView->getRenderTexture()->getSprite()->setScale(zoom / director->getContentScaleFactor());
        m_gameView->getRenderTexture()->getSprite()->setPosition(midWindow);
        m_gameView->getSelfLightingNode()->setScale(zoom / director->getContentScaleFactor());
        m_gameView->getSelfLightingNode()->setPosition(midWindow - (m_cameraModel->getPosition() * zoom));
    }
    else
    {
        cocos2d::Vec2 viewPosition = m_cameraController->getViewPosition();
        if (m_cameraModel->getPixelPerfet())
        {
            viewPosition = cocos2d::Vec2(std::floor(viewPosition.x),
                                         std::floor(viewPosition.y));
        }
        m_gameView->getView()->setPosition(viewPosition);
        m_gameView->getView()->setScale(zoom);
    }

    if (deferredRenderSetting.asBool())
    {
        const cocos2d::Value& renderLightingSetting = m_gameSettings->getValue(LightController::SETTING_RENDER_LIGHTING, cocos2d::Value(true));
        if (renderLightingSetting.asBool())
        {
            m_lightController->update(deltaTime);
            m_lightController->getLightMapTexture()->getSprite()->setScale(zoom / director->getContentScaleFactor());
            m_lightController->getLightMapTexture()->getSprite()->setPosition(midWindow);
        }
        
        m_gameView->renderToTexture();
    }
    
    m_audioController->setListenerPosition(m_cameraModel->getPosition());
}

void GameViewController::updatePlayerAnimations(const uint8_t playerID,
                                                std::shared_ptr<EntityView>& entityView,
                                                const EntitySnapshot& entitySnapshot,
                                                const PlayerState& playerState)
{
    const bool isReloading = playerState.animationState == AnimationState::Reload;
    const auto& currentAnimation = entityView->getSprite()->getActionByTag(EntityView::ANIMATION_TAG);
    const auto& currentAnimationState = entityView->getAnimation(currentAnimation);
    const bool switchingState = playerState.animationState != currentAnimationState;
    // Legs animation
    if (switchingState && !isReloading)
    {
        entityView->runAnimation(playerState.animationState);
        
        // Head animation
        if (playerState.animationState == Grab)
        {
            auto headSprite = entityView->getSecondarySprites().at(EntityView::HEAD_INDEX);
            headSprite->stopAllActions();
            headSprite->runAction(cocos2d::Sequence::create(cocos2d::MoveBy::create(0.06f, cocos2d::Vec2(0,-1)),
                                                            cocos2d::MoveBy::create(0.12f, cocos2d::Vec2(0,1)),
                                                            NULL));
        }
    }
    
    // Arms animation
    AnimationState armLanim = AnimationState::IdleArmL;
    AnimationState armRanim = AnimationState::IdleArmR;
    
    if (isReloading)
    {
        armRanim = AnimationState::Reload;
    }
    else if (playerState.weaponSlots.at(playerState.activeWeaponSlot).type) // Holding items overrides most right arm animations
    {
        if (playerState.flipX)
        {
            armLanim = AnimationState::HoldArmL;
        }
        else
        {
            armRanim = AnimationState::HoldArmR;
        }
    }
    else if (playerState.animationState == AnimationState::Walk ||
             playerState.animationState == AnimationState::Run)
    {
        armLanim = AnimationState::WalkArmL;
        armRanim = AnimationState::WalkArmR;
    }
    else if (playerState.animationState == AnimationState::Grab)
    {
        armRanim = AnimationState::WalkArmR;
    }

    auto currentLArmAction = entityView->getSecondarySprites().at(EntityView::ARM_LEFT_INDEX)->getActionByTag(EntityView::ANIMATION_TAG);
    auto currentRArmAction = entityView->getSecondarySprites().at(EntityView::ARM_RIGHT_INDEX)->getActionByTag(EntityView::ANIMATION_TAG);

    const bool isSwitchingLArmAnimation = armLanim != entityView->getAnimation(currentLArmAction);
    const bool isSwitchingRArmAnimation = armRanim != entityView->getAnimation(currentRArmAction);
    const bool wasReloading = !isReloading && entityView->getAnimation(currentRArmAction) == AnimationState::Reload;
    if (isSwitchingLArmAnimation)
    {
        entityView->getSecondarySprites().at(EntityView::ARM_LEFT_INDEX)->stopAllActions();
        entityView->getSecondarySprites().at(EntityView::ARM_LEFT_INDEX)->runAction(entityView->getAnimations().at(armLanim));
    }
    if (isSwitchingRArmAnimation)
    {
        auto rightArmSprite = entityView->getSecondarySprites().at(EntityView::ARM_RIGHT_INDEX);
        rightArmSprite->stopAllActions();
        rightArmSprite->runAction(entityView->getAnimations().at(armRanim));
        if (isReloading)
        {
            m_audioController->playPositionalAudio("SFX_Gun_Reload_1.wav", rightArmSprite->getPosition());
        }
        else if (wasReloading)
        {
            m_audioController->playPositionalAudio("SFX_Gun_Reload_2.wav", rightArmSprite->getPosition());
        }
    }
    
    auto headSprite = entityView->getSecondarySprites().at(EntityView::HEAD_INDEX);
    const auto currentHeadAction = headSprite->getActionByTag(EntityView::ANIMATION_TAG);
    const bool isIdleFaced = entityView->getAnimation(currentHeadAction) == AnimationState::IdleFace;
    const bool isAimFaced = entityView->getAnimation(currentHeadAction) == AnimationState::AimFace;
    const bool isUnAimFaced = entityView->getAnimation(currentHeadAction) == AnimationState::UnAimFace;
    const bool shouldAimFace = !isReloading &&
                                playerState.animationState == AnimationState::AimFace &&
                                playerState.weaponSlots.at(playerState.activeWeaponSlot).type;
    if (shouldAimFace && !isAimFaced)
    {
        headSprite->stopAllActions();
        headSprite->runAction(entityView->getAnimations().at(AnimationState::AimFace));
    }
    else if (!shouldAimFace && isAimFaced)
    {
        headSprite->stopAllActions();
        headSprite->runAction(entityView->getAnimations().at(AnimationState::UnAimFace));
    }
    else if (!shouldAimFace && !isIdleFaced && !isUnAimFaced)
    {
        headSprite->stopAllActions();
        headSprite->runAction(entityView->getAnimations().at(AnimationState::IdleFace));
    }

    if (playerState.flipX != entityView->getSprite()->isFlippedX())
    {
        entityView->setFlippedX(playerState.flipX);
        auto leftArm = entityView->getSecondarySprites().at(EntityView::ARM_LEFT_INDEX);
        if (playerState.flipX)
        {
            leftArm->setAnchorPoint(EntityView::ARM_L_ANCHOR_FLIPPED);
            leftArm->setPosition(EntityView::ARM_L_POS_FLIPPED);
        }
        else
        {
            leftArm->setAnchorPoint(EntityView::ARM_L_ANCHOR);
            leftArm->setPosition(EntityView::ARM_L_POS);
        }
    }

    float legAnimationSpeed = 0.f;
    float armAnimationSpeed = 0.f;

    if (playerState.animationState == AnimationState::Run)
    {
        legAnimationSpeed = 2.f;
        armAnimationSpeed = 2.f;
    }
    else if (playerState.animationState == AnimationState::Walk)
    {
        legAnimationSpeed = 1.f;
        armAnimationSpeed = 0.5f;
    }
    cocos2d::Speed* walkAnim = static_cast<cocos2d::Speed*>(entityView->getAnimations()[Walk].get());
    walkAnim->setSpeed(legAnimationSpeed);
    cocos2d::Speed* walkArmRAnim = static_cast<cocos2d::Speed*>(entityView->getAnimations()[WalkArmR].get());
    walkArmRAnim->setSpeed(armAnimationSpeed);
    cocos2d::Speed* walkArmLAnim = static_cast<cocos2d::Speed*>(entityView->getAnimations()[WalkArmL].get());
    walkArmLAnim->setSpeed(armAnimationSpeed);
}

void GameViewController::updateHeldItem(std::shared_ptr<EntityView>& entityView,
                                        const PlayerState& state)
{
    const EntityType heldItemType = (EntityType)state.weaponSlots.at(state.activeWeaponSlot).type;
    const bool hasHeldItem = heldItemType != EntityType::NoEntity;
    const bool hasSprite = entityView->getSecondarySprites().count(EntityView::WEAPON_INDEX) > 0;
    
    if (!hasHeldItem)
    {
        if (hasSprite)
        {
            entityView->getSecondarySprites()[EntityView::WEAPON_INDEX]->removeFromParent();
            entityView->removeSecondarySprite(EntityView::WEAPON_INDEX);
        }
        return;
    }
        
    const auto& itemData = EntityDataModel::getStaticEntityData(heldItemType);
    const bool hasWrongType = !hasSprite ? false : entityView->getSecondarySprites()[EntityView::WEAPON_INDEX]->getTag() != heldItemType;
    if (!hasSprite || hasWrongType)
    {
        std::string spriteFrame = itemData.sprite;
        if (heldItemType  == EntityType::Item_RPG)
        {
            const auto& weapon = state.weaponSlots.at(state.activeWeaponSlot);
            if (weapon.amount > 0)
            {
                spriteFrame = "Weapon-RPG-Loaded.png";
            }
        }
        entityView->setupSecondarySprite(spriteFrame, EntityView::WEAPON_INDEX);
        entityView->getSecondarySprites()[EntityView::WEAPON_INDEX]->setTag(heldItemType);
        entityView->getSecondarySprites()[EntityView::WEAPON_INDEX]->setAnchorPoint(itemData.anchor);

        if (!hasSprite)
        {
            entityView->getSprite()->addChild(entityView->getSecondarySprites()[EntityView::WEAPON_INDEX], 1);
        }
    }
    cocos2d::RefPtr<cocos2d::Sprite> heldItem = entityView->getSecondarySprites()[EntityView::WEAPON_INDEX];
    const cocos2d::Vec2 aimPosition = cocos2d::Vec2(state.aimPointX, state.aimPointY);
    const WeaponConstants::WeaponStateData data = WeaponConstants::getWeaponData(entityView->getSprite()->getPosition(),
                                                                                 aimPosition,
                                                                                 itemData.weapon.holdOffset,
                                                                                 itemData.weapon.projectileOffset,
                                                                                 state.flipX);
    
    heldItem->setFlippedX(state.flipX);
    heldItem->setRotation(data.weaponAngle);
    heldItem->setPosition(cocos2d::Vec2(5, 7) + data.weaponPosition);
    
    cocos2d::RefPtr<cocos2d::Sprite> leftArm = entityView->getSecondarySprites()[EntityView::ARM_LEFT_INDEX];
    cocos2d::RefPtr<cocos2d::Sprite> rightArm = entityView->getSecondarySprites()[EntityView::ARM_RIGHT_INDEX];
    if (state.flipX)
    {
        leftArm->setRotation(data.weaponAngle);
        rightArm->setRotation(0.f);
        heldItem->setLocalZOrder(-1);
    }
    else
    {
        leftArm->setRotation(0.f);
        rightArm->setRotation(data.weaponAngle);
        heldItem->setLocalZOrder(1);
    }
}

void GameViewController::updateEntities(const SnapshotData& snapshot)
{
    // Remove entity views for removed entities
    std::vector<uint32_t> removedEntityIDs;
    for (const auto& pair : m_entityViews)
    {
        if (snapshot.entityData.find(pair.first) == snapshot.entityData.end())
        {
            removedEntityIDs.push_back(pair.first);
        }
    }
    for (const uint32_t removedEntityID : removedEntityIDs)
    {
        onEntityDestroyed(removedEntityID, snapshot);
    }

    // Spawn new entity views/update entity views (positions, rotations etc.)
    const cocos2d::Size mapSize = m_levelModel->getMapSize();
    for (const auto& pair : snapshot.entityData)
    {
        const uint32_t entityID = pair.first;
        if (m_entityViews.find(entityID) == m_entityViews.end())
        {
            onEntitySpawned(entityID, pair.second);
            continue;
        }
        auto& entityView = m_entityViews[entityID];
        entityView->getSprite()->setPosition(pair.second.positionX, pair.second.positionY);
        entityView->getSprite()->setLocalZOrder(mapSize.height - pair.second.positionY);
        entityView->getSprite()->setRotation(pair.second.rotation * (180.0 / M_PI));
    }
    
    // Update player state data (animtions, held items)
    for (const auto& data : snapshot.playerData)
    {
        const uint32_t entityID = data.second.entityID;
        if (m_entityViews.find(entityID) == m_entityViews.end())
        {
            CCLOG("GameViewController::updateEntities error, no view found for PlayerID: %i", data.first);
            continue;
        }
        
        auto& entityView = m_entityViews[entityID];
        const EntitySnapshot& entitySnapshot = snapshot.entityData.at(data.second.entityID);
        updatePlayerAnimations(data.first, entityView, entitySnapshot, data.second);
        updateHeldItem(entityView, data.second);
    }
}

void GameViewController::updateCursor(const SnapshotData& snapshot)
{
    const float zoom = m_cameraModel->getZoom();
    const cocos2d::Vec2 cursorPosition = m_inputModel->getMouseCoord();
    m_hudView->getCrosshairView()->getCrossHair()->setPosition(cursorPosition);
    m_hudView->getCrosshairView()->getCrossHair()->setScale(zoom);

    if (m_cameraModel->getFollowPlayer())
    {
        const auto playerIt = snapshot.playerData.find(m_cameraModel->getCameraFollowPlayerID());
        if (playerIt == snapshot.playerData.end())
        {
            return;
        }
        const PlayerState& playerState = playerIt->second;
        const EntitySnapshot& entitySnapshot = snapshot.entityData.at(playerState.entityID);

        if (m_shotHitLastFrame)
        {
            m_hudView->getCrosshairView()->setMode(CrosshairView::CrosshairMode::HITMARK);
        }
        else
        {
            const auto& weapon = playerState.weaponSlots.at(playerState.activeWeaponSlot);
            if (weapon.type != EntityType::NoEntity &&
                playerState.health > 0.f)
            {
                auto itemData = EntityDataModel::getStaticEntityData((EntityType)weapon.type);

                m_hudView->getCrosshairView()->setMode(m_inputModel->getInputValue(InputConstants::ACTION_AIM) ?
                                                       CrosshairView::CrosshairMode::NARROW :
                                                       CrosshairView::CrosshairMode::WIDE);
                float targetZoom = CameraModel::DEFAULT_ZOOM_LEVEL;
                if (m_inputModel->getInputValue(InputConstants::ACTION_AIM))
                {
                    if (itemData.weapon.type == WeaponType::Weapon_Type_Sniper)
                    {
                        targetZoom = 1.f;
                    }
                    else if (itemData.weapon.type == WeaponType::Weapon_Type_AR)
                    {
                        targetZoom = 2.5f;
                    }
                    else if (itemData.weapon.type == WeaponType::Weapon_Type_SMG)
                    {
                        targetZoom = 3.f;
                    }
                    else if (itemData.weapon.type == WeaponType::Weapon_Type_Pistol)
                    {
                        targetZoom = 3.5f;
                    }
                }

                m_cameraModel->setTargetZoom(targetZoom);
            }
        }
        
        auto& entityView = m_entityViews[playerState.entityID];
        const cocos2d::Vec2 playerPosition = entityView ? entityView->getSprite()->getPosition() : cocos2d::Vec2(entitySnapshot.positionX, entitySnapshot.positionY);
        const cocos2d::Vec2 aimPosition = getAimPosition(m_inputModel->getMouseCoord());

        uint32_t entityUnderCursorID = 0;
        bool itemsInRange = false;
        for (const auto& entityPair : snapshot.entityData)
        {
            if (entityPair.second.type == EntityType::PlayerEntity)
            {
                continue;
            }
            
            const cocos2d::Vec2 position = cocos2d::Vec2(entityPair.second.positionX, entityPair.second.positionY);
            if (position.distance(playerPosition) <= ITEM_GRAB_RADIUS)
            {
                itemsInRange = true;

                const auto& collisionRects = EntityDataModel::getCollisionRects((EntityType)entityPair.second.type);
                for (auto baseRect : collisionRects)
                {
                    const cocos2d::Rect rect = cocos2d::Rect(baseRect.origin + position, baseRect.size);
                    if (rect.containsPoint(aimPosition))
                    {
                        entityUnderCursorID = entityPair.first;
                        break;
                    }
                }
            }
        }

        if (itemsInRange)
        {
            const double time = cocos2d::utils::gettime();
            const float glow = 0.5f + (std::abs(std::sin(time)) * 0.5f);
            m_gameView->getPixelDrawNode()->drawCircle(playerPosition, ITEM_GRAB_RADIUS, 0.f, 64.f, false, 1.f, 1.f, cocos2d::Color4F(1.f,1.f,1.f, glow));
        }

        if (entityUnderCursorID != 0)
        {
            const EntitySnapshot& entitySnapshot = snapshot.entityData.at(entityUnderCursorID);
            const cocos2d::Vec2 entityPosition = cocos2d::Vec2(entitySnapshot.positionX, entitySnapshot.positionY);

            cocos2d::Rect rect = cocos2d::Rect(entityPosition, cocos2d::Size::ZERO);
            const auto& collisionRects = EntityDataModel::getCollisionRects((EntityType)entitySnapshot.type);
            for (const auto& baseRect : collisionRects)
            {
                if (rect.size.width < baseRect.size.width)
                {
                    rect.size.width = baseRect.size.width;
                }
                rect.size.height += baseRect.size.height;
            }
            rect.origin -= rect.size * 0.5f;
            const cocos2d::Vec2 offset = cocos2d::Vec2(0.f, rect.size.height + 8.f);
            const cocos2d::Vec2 labelPosition = m_gameView->toViewPosition(entityPosition + offset);
            const StaticEntityData& entityData = EntityDataModel::getStaticEntityData((EntityType)entitySnapshot.type);

            const double time = cocos2d::utils::gettime();
            const float glow = 0.5f + (std::abs(std::sin(time)) * 0.5f);
            m_gameView->getPixelDrawNode()->drawRect(rect.origin, rect.origin + rect.size, cocos2d::Color4F(1.f,1.f,1.f, glow));

            m_hudView->showHighlightLabel(entityData.name, labelPosition);
            m_hudView->getHighlightLabel()->setScale(2.f);
        }
        else
        {
            m_hudView->hideHighlightLabel();
        }
    }
}

void GameViewController::updateShotTrails(const float deltaTime)
{
    const float TRAIL_FADEOUT_FACTOR = 6.f;
    for (auto it = m_shotTrails.begin(); it != m_shotTrails.end();)
    {
        ShotTrail& trail = (*it);
        trail.time -= deltaTime * TRAIL_FADEOUT_FACTOR;
        if (trail.time <= 0.f)
        {
            it = m_shotTrails.erase(it);
        }
        else
        {
            m_gameView->getPixelDrawNode()->drawLine(trail.start, trail.end, cocos2d::Color4F(.5f, .5f, .5f, trail.time));
            it++;
        }
    }
}

void GameViewController::updatePostProcess(const float zoomRadius)
{
    const bool hasLightMap = m_lightController->getLightMapTexture() != nullptr;
    cocos2d::GLProgram* program = hasLightMap ? m_postProcessShader : m_postProcessNoLightShader;
    cocos2d::GLProgramState* postProcessState = cocos2d::GLProgramState::getOrCreateWithGLProgram(program);

    if (hasLightMap)
    {
        postProcessState->setUniformTexture("u_lightTexture", m_lightController->getLightMapTexture()->getSprite()->getTexture());
    }

    const cocos2d::Vec2 winSize = cocos2d::Director::getInstance()->getWinSize();
    const cocos2d::Vec2 midView = winSize * 0.5f;
    const cocos2d::Vec2 midScreenToCursor = m_inputModel->getMouseCoord() - midView;
    const cocos2d::Vec2 viewPos = m_gameView->getView()->getPosition();
    const cocos2d::Vec2 cursorScreenPosition = midView + (midScreenToCursor / m_cameraModel->getZoom());

    postProcessState->setUniformVec2("u_resolution", winSize);
    postProcessState->setUniformVec2("u_cursor", cursorScreenPosition);
    postProcessState->setUniformFloat("u_radius", zoomRadius);
    m_gameView->getRenderTexture()->getSprite()->setGLProgram(program);
}

void GameViewController::renderPostProcess(const SnapshotData& snapshot)
{
    // Sniper scope zoom post-process
    const auto playerIt = snapshot.playerData.find(m_cameraModel->getCameraFollowPlayerID());
    if (playerIt == snapshot.playerData.end())
    {
        return;
    }
    const PlayerState& playerState = playerIt->second;
    const EntityType heldItemType = (EntityType)playerState.weaponSlots.at(playerState.activeWeaponSlot).type;
    const bool hasItem = heldItemType != EntityType::NoEntity;
    
    float zoomRadius = 0.f;
    if (hasItem)
    {
        const auto& itemData = EntityDataModel::getStaticEntityData(heldItemType);
        if (m_inputModel->getInputValue(InputConstants::ACTION_AIM) > 0.5f &&
            itemData.weapon.type == WeaponType::Weapon_Type_Sniper)
        {
            zoomRadius = 64.f;
        }
    }
    updatePostProcess(zoomRadius);
    
    const cocos2d::Value& losRenderSetting = m_gameSettings->getValue(GameViewConstants::SETTING_RENDER_LINE_OF_SIGHT, cocos2d::Value(true));
    if (!losRenderSetting.asBool())
    {
        return;
    }
    // LINE OF SIGHT
    const EntitySnapshot& entitySnapshot = snapshot.entityData.at(playerState.entityID);
    const cocos2d::Vec2 playerPos = cocos2d::Vec2(entitySnapshot.positionX, entitySnapshot.positionY);
    m_gameView->updateLineOfSight(playerPos, m_levelModel->getStaticRects());
}

void GameViewController::renderDebug()
{
//    const cocos2d::Vec2 viewPos = m_gameView->getView()->getPosition();
//    const cocos2d::Value& deferredRenderSetting = m_gameSettings->getValue(GameView::SETTING_RENDER_DEFERRED, cocos2d::Value(true));
//    const float viewScale = deferredRenderSetting.asBool() ? m_gameView->getRenderTexture()->getSprite()->getScale() : m_gameView->getView()->getScale();
    // TODO: Toggle these loops below with some settings
//
//    // Debug render local entities collision boxes
//    const auto& entities = m_entitiesModel->getEntities();
//    for (const auto& entityPair : entities)
//    {
//        const auto& entity = entityPair.second;
//        // For each collision rect in this entity
//        for (const cocos2d::Rect& baseRect : entity->getCollisionRects())
//        {
//            const cocos2d::Vec2 pos = m_gameView->toViewPosition(baseRect.origin + entity->getPosition());
//            const cocos2d::Rect rect = cocos2d::Rect(pos, baseRect.size * viewScale);
//
//            m_gameView->getDebugDrawNode()->drawRect(rect.origin,
//                                                     cocos2d::Vec2(rect.getMaxX(), rect.getMaxY()),
//                                                     cocos2d::Color4F(1.f,1.f,1.f,0.5f));
//        }
//    }
        // Debug render static level collision boxes
//        const auto& walls = m_levelModel->getStaticRects();
//        for (const auto& rect : walls)
//        {
//            cocos2d::Vec2 intersect = CollisionUtils::closestPointOnBoundsToPoint(rayEnd, rect);
//            if (rect.containsPoint(intersect))
//            {
//                m_gameView->getPixelDrawNode()->drawCircle(intersect, 2.f, 0.f, 8, false, 1.f, 1.f, cocos2d::Color4F::RED);
//            }
//            float fraction = CollisionUtils::getRayIntersectionFractionOfFirstRay(rayStart, rayEnd, rect.origin, rect.origin + cocos2d::Vec2(rect.size.width, 0));
//            if (fraction < 1.f)
//            {
//                cocos2d::Vec2 ray = (rayEnd - rayStart) * fraction;
//                m_gameView->getPixelDrawNode()->drawCircle(rayStart + ray, 2.f, 0.f, 8, false, 1.f, 1.f, cocos2d::Color4F::YELLOW);
//            }
//             Debug intersection with walls
//            float h = CollisionUtils::getRayIntersectionFraction(rayStart, (rayEnd - rayStart), rect);
//            if (h != std::numeric_limits<float>::max())
//            {
//                cocos2d::Vec2 ray = (rayEnd - rayStart) * h;
//                m_gameView->getPixelDrawNode()->drawCircle(rayStart + ray, 2.f, 0.f, 8.f, false, 1.f, 1.f, cocos2d::Color4F::RED);
//            }
//
//            const cocos2d::Rect viewRect = cocos2d::Rect(rect.origin * viewScale, rect.size * viewScale);
//            m_gameView->getDebugDrawNode()->drawRect(viewRect.origin + viewPos,
//                                                     cocos2d::Vec2(viewRect.getMaxX(), viewRect.getMaxY()) + viewPos,
//                                                     cocos2d::Color4F(0.0f, 0.5f, 0.5f, 1.f));
//        }
}

void GameViewController::renderPlayerDeath(const cocos2d::Vec2& position,
                                           const cocos2d::Vec2& direction,
                                           const bool headShot)
{
    static const std::vector<std::string> REGULAR_SPRITES = {
        "ArmLIdle-0.png",
        "ArmRIdle-0.png",
        "BaseHead.png",
        "TorsoIdle-0.png",
    };
    static const std::vector<std::string> GORE_SPRITES = {
        "Gore-Hand1.png",
        "Gore-Hand2.png",
        "Gore-Head1.png",
        "Gore-Head2.png",
        "Gore-Torso1.png",
        "Gore-Torso2.png",
    };
    
    const size_t handSpritesCount = 2;
    for (size_t i = 0; i < handSpritesCount; i++)
    {
        const size_t goreSprite = cocos2d::random(0, 1);
        float randomX = cocos2d::random(-4.f, 4.f);
        float randomY = cocos2d::random(0.f, 16.f);
        float randomZ = cocos2d::random(2.f, 22.f);
        const cocos2d::Vec2 randomPos = cocos2d::Vec2(randomX, randomY);
        const std::string& sprite = goreSprite ? GORE_SPRITES.at(goreSprite) : REGULAR_SPRITES.at(i);
        m_gameView->createPseudo3DSprite(sprite,
                                         position + randomPos,
                                         randomZ,
                                         randomPos + direction * 16.f,
                                         16.f,
                                         10.f,
                                         0.5f);
    }
    
    const size_t torsoSprite = cocos2d::random(0, 1);
    float randomX = cocos2d::random(-4.f, 4.f);
    float randomY = cocos2d::random(0.f, 16.f);
    float randomZ = cocos2d::random(2.f, 22.f);
    const cocos2d::Vec2 randomPos = cocos2d::Vec2(randomX, randomY);

    m_gameView->createPseudo3DSprite(GORE_SPRITES.at(4 + torsoSprite),
                                     position + randomPos,
                                     randomZ,
                                     randomPos + direction * 16.f,
                                     16.f,
                                     10.f,
                                     0.5f);
    
    if (headShot)
    {
        const size_t headSprite = cocos2d::random(0, 1);
        float randomX = cocos2d::random(-4.f, 4.f);
        float randomY = cocos2d::random(0.f, 16.f);
        float randomZ = cocos2d::random(2.f, 22.f);
        const cocos2d::Vec2 randomPos = cocos2d::Vec2(randomX, randomY);

        m_gameView->createPseudo3DSprite(GORE_SPRITES.at(2 + headSprite),
                                         position + randomPos,
                                         randomZ,
                                         randomPos + direction * 16.f,
                                         16.f,
                                         10.f,
                                         0.5f);
    }
    else
    {
        m_gameView->createPseudo3DSprite(REGULAR_SPRITES.at(2),
                                         position,
                                         0.f,
                                         direction * 16.f,
                                         16.f,
                                         10.f,
                                         0.5f);
    }

    
    for (int i = 0; i < 16; i++)
    {
        float randomX = cocos2d::random(-2.f, 2.f);
        float randomY = cocos2d::random(-2.f, 2.f);
        float randomZ = cocos2d::random(-2.f, 2.f);
        const cocos2d::Vec2 randomPos = cocos2d::Vec2(randomX, randomY);
        const cocos2d::Vec2 bloodParticlePos = position + randomPos;
        
        m_gameView->createPseudo3DParticle(cocos2d::Color4F::RED,
                                           bloodParticlePos,
                                           2.f * (randomZ + 2.f),
                                           randomPos * 16.f,
                                           32.f,
                                           10.f,
                                           0.f);
    }
}

cocos2d::Vec2 GameViewController::getAimPosition(const cocos2d::Vec2& mouseCoord) const
{
    const cocos2d::Vec2 viewPosition = m_cameraController->getViewPosition();
    return (mouseCoord - viewPosition) / m_cameraModel->getZoom();
}
