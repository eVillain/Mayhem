#ifndef InventoryLayer_h
#define InventoryLayer_h

#include "Network/NetworkMessages.h"
#include "ViewLayer.h"
#include "Game/Shared/EntityConstants.h"

#include "GameTableView.h"
#include "cocos2d.h"
#include "cocos-ext.h"

class SnapshotModel;

class InventoryLayer
: public ViewLayer
, public cocos2d::extension::TableViewDataSource
, public cocos2d::extension::TableViewDelegate
{
public:
    static const std::string DESCRIPTOR;
    InventoryLayer(std::shared_ptr<SnapshotModel> snapshotModel);
    ~InventoryLayer();
    
    cocos2d::Node* getRoot() override { return m_layer; }
    const std::string& getDescriptor() const override { return DESCRIPTOR; }
    void update(const float deltaTime) override {};
    
    void initialize();
    void setData(const uint8_t playerID);
    void clearData();
    
    void setItemPickupCallback(std::function<void(const EntityType type,
                                                  const uint16_t amount,
                                                  const uint16_t entityID)> callback) { m_pickupCallback = callback; }
        
    // TableViewDataSource
    cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView* table, ssize_t idx) override;
    cocos2d::Size cellSizeForTable(cocos2d::extension::TableView* table) override;
    cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx) override;
    ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table) override;
    // TableViewDelegate
    void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) override;
    
private:
    static const cocos2d::Size CELL_SIZE;
    std::shared_ptr<SnapshotModel> m_snapshotModel;
    uint8_t m_playerID;
    std::function<void(const EntityType type, const uint16_t amount, const uint16_t entityID)> m_pickupCallback;
    std::vector<uint32_t> m_nearEntities;

    cocos2d::RefPtr<cocos2d::LayerColor> m_layer;
    
    cocos2d::RefPtr<GameTableView> m_myStuffTable;
    cocos2d::RefPtr<GameTableView> m_nearTable;
    
    cocos2d::Node* createItemBox();
    const std::vector<uint32_t> getNearEntities(const std::map<uint32_t, EntitySnapshot>& entityData,
                                                const uint32_t ignoreEntityID,
                                                const cocos2d::Vec2 position,
                                                const float radius) const;
};

#endif /* InventoryLayer_h */
