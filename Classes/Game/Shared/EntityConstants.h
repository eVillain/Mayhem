#ifndef EntityConstants_h
#define EntityConstants_h

#include "cocos2d.h"
#include <string>

static constexpr float BULLET_PISTOL_VELOCITY = 1400.0f;
static constexpr float BULLET_MACHINEGUN_VELOCITY = 1200.0f;
static constexpr float BULLET_SHOTGUN_VELOCITY = 1000.0f;
static constexpr float BULLET_ROCKET_VELOCITY = 400.0f;
static constexpr int RAILGUN_DAMAGE = 100.0f;

static constexpr float PLAYER_WALK_VEL = 120.f;
static constexpr float PLAYER_RUN_VEL = 180.f;
static constexpr float PLAYER_MAX_VEL = 200.f;
static constexpr float PLAYER_IDLE_VEL_THRESHOLD = 60.f;
static constexpr float PLAYER_DEFAULT_HEALTH = 100.f;

typedef enum
{
    NoEntity = 0,
    
    Item_First_Placeholder, // 1
    Item_Deagle, // 2
    Item_Revolver, // 3
    Item_Uzi, // 4
    Item_Vector, // 5
    Item_AKM, // 6
    Item_M416, // 7
    Item_SawedOff, // 8
    Item_AWM, // 9
    Item_RPG, // 10
    Item_Railgun, // 11
    Item_Kar98, // 12
    Item_Nade_Frag = 17,
    Item_Nade_Smoke = 18,

    Item_Helmet_Level1 = 70, // unimplemented
    Item_Helmet_Level2 = 71,
    Item_Helmet_Level3 = 72,
    Item_Vest_Level1 = 73, // unimplemented
    Item_Vest_Level2 = 74, // unimplemented
    Item_Vest_Level3 = 75, // unimplemented
    
    Item_Ammo_9mm = 80,
    Item_Ammo_45ACP = 81,
    Item_Ammo_556 = 82,
    Item_Ammo_762 = 83,
    Item_Ammo_Shells = 84,
    Item_Ammo_RPG = 85,
    Item_Ammo_Slugs = 86,
    Item_Last_Placeholder,
    
    Projectile_Bullet = 93,
    Projectile_Rocket = 94,
    Projectile_Grenade = 95,
    Projectile_Smoke = 96,

    Loot_Box = 100,
    
    ExplosionEntity = 200,
    
    PlayerEntity = 254,
    Death = 255
} EntityType;

typedef enum
{
    Weapon_Type_None = 0,
    Weapon_Type_Pistol,
    Weapon_Type_SMG,
    Weapon_Type_AR,
    Weapon_Type_Shotgun,
    Weapon_Type_Sniper,
    Weapon_Type_Launcher,
    Weapon_Type_Throwable,
} WeaponType;

typedef enum
{
    Damage_Type_None = 0,
    Damage_Type_Hitscan,
    Damage_Type_Projectile,
    Damage_Type_Explosion,
} DamageType;

typedef enum
{
    Armor_Type_Helmet = 0,
    Armor_Type_Vest,
} ArmorType;

typedef enum
{
    Ammo_Type_None = 0,
    Ammo_Type_9mm = Item_Ammo_9mm,
    Ammo_Type_45ACP = Item_Ammo_45ACP,
    Ammo_Type_556 = Item_Ammo_556,
    Ammo_Type_762 = Item_Ammo_762,
    Ammo_Type_Shells = Item_Ammo_Shells,
    Ammo_Type_RPG = Item_Ammo_RPG,
    Ammo_Type_Slug = Item_Ammo_Slugs,
    Ammo_Type_Frag = Item_Nade_Frag,
    Ammo_Type_Smoke = Item_Nade_Smoke,
} AmmoType;

typedef enum
{
    Fire_Mode_Type_None = 0,
    Fire_Mode_Type_Single,
    Fire_Mode_Type_Burst,
    Fire_Mode_Type_Auto,
} FireModeType;

enum CharacterType
{
    Character_Base = 0,
};

enum AnimationState
{
    Idle = 0,
    Walk,
    Jump,
    IdleArmL,
    IdleArmR,
    WalkArmL,
    WalkArmR,
    HoldArmL,
    HoldArmR,
    Grab,
    Run,
    Hurt,
    Dying1,
    Dying2,
    Reload,
    Attack,
    IdleFace,
    AimFace,
    UnAimFace,
};

enum WeaponSlot {
    MAIN_1 = 0,
    MAIN_2,
    SECONDARY,
    THROWABLE,
    MELEE
};

struct InventoryItem {
    EntityType type;
    uint16_t amount;
};

struct WeaponData
{
    WeaponType type;
    std::string sound;
    cocos2d::Vec2 holdOffset;
    cocos2d::Vec2 projectileOffset;
    DamageType damageType;
    EntityType projectileType;
    int damageAmount;
    float recoil;
    float timeShot;
    float timeReload;
    std::vector<FireModeType> fireModes;
};

struct AmmoData
{
    AmmoType type;
    int amount;
};

struct ArmorData
{
    ArmorType type;
    int level;
    int amount;
};

struct StaticEntityData
{
    std::string name;
    std::string sprite;
    std::vector<cocos2d::Rect> rects;
    cocos2d::Vec2 anchor;
    WeaponData weapon;
    AmmoData ammo;
    ArmorData armor;
};

static const EntityType getArmorType(const ArmorType type, const int level)
{
    return (EntityType)(Item_Helmet_Level1 + (type * 3) + level);
}

#endif /* EntityConstants_h */
