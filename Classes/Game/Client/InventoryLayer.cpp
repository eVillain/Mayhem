#include "InventoryLayer.h"
#include "HUDHelper.h"
#include "EntityDataModel.h"
#include "SharedConstants.h"
#include "PlayerLogic.h"
#include "SnapshotModel.h"

const std::string InventoryLayer::DESCRIPTOR = "Inventory";
const cocos2d::Size InventoryLayer::CELL_SIZE = cocos2d::Size(160, 30);

InventoryLayer::InventoryLayer(std::shared_ptr<SnapshotModel> snapshotModel)
: m_snapshotModel(snapshotModel)
, m_playerID(0)
, m_pickupCallback(nullptr)
, m_layer(nullptr)
, m_myStuffTable(nullptr)
, m_nearTable(nullptr)
{
}

InventoryLayer::~InventoryLayer()
{
}

void InventoryLayer::initialize()
{
    const cocos2d::Size winSize = cocos2d::Director::getInstance()->getWinSize();

    m_layer = cocos2d::LayerColor::create(cocos2d::Color4B(0,0,0,127));
    
    const cocos2d::Size TABLE_SIZE = cocos2d::Size(CELL_SIZE.width, winSize.height);
    m_myStuffTable = GameTableView::create();
    m_nearTable = GameTableView::create();
    m_myStuffTable->configure(TABLE_SIZE,
                              cocos2d::Vec2(CELL_SIZE.width + 16, 0),
                              this,
                              this,
                              "My Stuff");
    m_nearTable->configure(TABLE_SIZE,
                           cocos2d::Vec2(8, 0),
                           this,
                           this,
                           "Nearby");
        
    m_layer->addChild(m_myStuffTable);
    m_layer->addChild(m_nearTable);
}

void InventoryLayer::setData(const uint8_t playerID)
{
    m_playerID = playerID;
    
    if (m_snapshotModel->getSnapshots().empty())
    {
        return;
    }
        
    const auto& snapshot = m_snapshotModel->getSnapshots().back();
    auto playerIt = snapshot.playerData.find(playerID);
    if (playerIt == snapshot.playerData.end())
    {
        return;
    }
    const auto& playerState = playerIt->second;
    
    auto entityIt = snapshot.entityData.find(playerState.entityID);
    if (entityIt == snapshot.entityData.end())
    {
        return;
    }
    
    const cocos2d::Vec2 position = cocos2d::Vec2(entityIt->second.positionX,
                                                 entityIt->second.positionY);
    
    m_nearEntities = getNearEntities(snapshot.entityData,
                                     playerState.entityID,
                                     position,
                                     ITEM_GRAB_RADIUS);
    m_myStuffTable->getTable()->reloadData();
    m_nearTable->getTable()->reloadData();
}

void InventoryLayer::clearData()
{
    m_playerID = 0;
    m_nearEntities.clear();
}

cocos2d::Size InventoryLayer::tableCellSizeForIndex(cocos2d::extension::TableView* table, ssize_t idx)
{
    return CELL_SIZE;
}

cocos2d::Size InventoryLayer::cellSizeForTable(cocos2d::extension::TableView* table)
{
    return CELL_SIZE;
}

cocos2d::extension::TableViewCell* InventoryLayer::tableCellAtIndex(cocos2d::extension::TableView* table, ssize_t idx)
{
    const int CELL_LABEL_TAG = 555;
    cocos2d::extension::TableViewCell* cell = table->dequeueCell();
    if (!cell)
    {
        cell = cocos2d::extension::TableViewCell::create();
        cell->setContentSize(CELL_SIZE);
        
        auto box = createItemBox();
        box->setPosition(CELL_SIZE * 0.5f);
        cell->addChild(box);
                
        auto label = HUDHelper::createLabel5x7("", 18);
        label->setAlignment(cocos2d::TextHAlignment::LEFT);
        label->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
        label->setPosition(cocos2d::Vec2(8, CELL_SIZE.height * 0.5f));
        label->setTag(CELL_LABEL_TAG);
        cell->addChild(label);
    }

    if (m_snapshotModel->getSnapshots().empty())
    {
        return cell;
    }
        
    const auto& snapshot = m_snapshotModel->getSnapshots().back();

    std::string itemName;
    if (table == m_myStuffTable->getTable())
    {
        auto inventoryData = snapshot.inventory.at(idx);
        auto entityData = EntityDataModel::getStaticEntityData((EntityType)inventoryData.type);
        itemName = entityData.name;
    }
    else
    {
        auto entityIt = snapshot.entityData.find(m_nearEntities.at(idx));
        if (entityIt != snapshot.entityData.end())
        {
            auto entityData = EntityDataModel::getStaticEntityData((EntityType)entityIt->second.type);
            itemName = entityData.name;
        }
        else
        {
            itemName = "Error";
        }
    }
    
    cocos2d::Label* label = cell->getChildByTag<cocos2d::Label*>(CELL_LABEL_TAG);
    if (label)
    {
        label->setString(itemName);
    }

    return cell;
}

ssize_t InventoryLayer::numberOfCellsInTableView(cocos2d::extension::TableView* table)
{
    if (table == m_myStuffTable->getTable())
    {
        if (m_snapshotModel->getSnapshots().empty())
        {
            return 0;
        }
            
        auto& snapshot = m_snapshotModel->getSnapshots().back();
        return snapshot.inventory.size();
    }
    return m_nearEntities.size();
}

void InventoryLayer::tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell)
{
    if (cell->getIdx() < 0)
    {
        return;
    }
    if (m_snapshotModel->getSnapshots().empty())
    {
        return;
    }
        
    auto& snapshot = m_snapshotModel->getSnapshots().back();
    
    bool isMyTable = table == m_myStuffTable->getTable();
    CCLOG("tableCellTouched %s, %zi", isMyTable ? "mine" : "near", cell->getIdx());
    if (isMyTable)
    {
        auto& item = snapshot.inventory.at(cell->getIdx());
        if (item.type == EntityType::Item_Nade_Frag ||
            item.type == EntityType::Item_Nade_Smoke)
        {
            auto playerIt = snapshot.playerData.find(m_playerID);
            if (playerIt != snapshot.playerData.end())
            {
                auto& playerState = playerIt->second;
                const auto throwableType = playerState.weaponSlots.at(WeaponSlot::THROWABLE).type;
                if (throwableType != EntityType::PlayerEntity  &&
                    playerState.weaponSlots.at(WeaponSlot::THROWABLE).amount > 0)
                {
                    PlayerLogic::setInventoryAmount((EntityType)throwableType,
                                                    PlayerLogic::getInventoryAmount((EntityType)throwableType, snapshot.inventory) + playerState.weaponSlots.at(WeaponSlot::THROWABLE).amount,
                                                    snapshot.inventory);
                }
                playerState.weaponSlots.at(WeaponSlot::THROWABLE).type = item.type;
                playerState.weaponSlots.at(WeaponSlot::THROWABLE).amount = item.amount;
                item.amount = 0;
                m_myStuffTable->getTable()->reloadData();
            }
        }
    }
    else
    {
        if (m_pickupCallback)
        {
            auto entityIt = snapshot.entityData.find(m_nearEntities.at(cell->getIdx()));
            if (entityIt != snapshot.entityData.end())
            {
                auto entityData = EntityDataModel::getStaticEntityData((EntityType)entityIt->second.type);
                uint16_t amount = 1;
                if (EntityDataModel::isItemType((EntityType)entityIt->second.type))
                {
                    amount = entityIt->second.amount;
                }
                
                m_pickupCallback((EntityType)entityIt->second.type, amount, entityIt->first);
            }
        }
    }
}

cocos2d::Node* InventoryLayer::createItemBox()
{
    cocos2d::Node* itemNode = cocos2d::Node::create();
    itemNode->setAnchorPoint(cocos2d::Vec2::ZERO);
    
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> cellBG = HUDHelper::createHUDFill();
    cellBG->setColor(cocos2d::Color3B::GRAY);
    cellBG->setOpacity(63);
    cellBG->setPreferredSize(CELL_SIZE - cocos2d::Size(2,2));
    cellBG->setPosition(cocos2d::Vec2::ONE);
    itemNode->addChild(cellBG);
    
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> cellBorder = HUDHelper::createHUDRect();
    cellBorder->setColor(cocos2d::Color3B::GRAY);
    cellBorder->setPreferredSize(CELL_SIZE);
    itemNode->addChild(cellBorder);
    
    return itemNode;
}

const std::vector<uint32_t> InventoryLayer::getNearEntities(const std::map<uint32_t, EntitySnapshot>& entityData,
                                                            const uint32_t ignoreEntityID,
                                                            const cocos2d::Vec2 position,
                                                            const float radius) const
{
    std::vector<uint32_t> nearEntities;
    const float radiusSq = radius * radius;
    for (auto entityPair : entityData)
    {
        if (entityPair.first == ignoreEntityID)
        {
            continue;
        }
        const cocos2d::Vec2 pos = cocos2d::Vec2(entityPair.second.positionX,
                                                entityPair.second.positionY);
        if (pos.distanceSquared(position) <= radiusSq)
        {
            nearEntities.push_back(entityPair.first);
        }
    }
    
    return nearEntities;
}
