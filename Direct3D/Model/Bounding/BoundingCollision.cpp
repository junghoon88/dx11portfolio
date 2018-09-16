#include "stdafx.h"
#include "BoundingCollision.h"

#include "BoundingBox.h"
#include "BoundingCapsule.h"
#include "BoundingSphere.h"

#include "../../Units/GameUnit.h"

BoundingCollision* BoundingCollision::instance = NULL;

BoundingCollision::BoundingCollision()
{
}

BoundingCollision::~BoundingCollision()
{
}

void BoundingCollision::AddBoundings(vector<Bounding*>& boundings)
{
	size_t count = boundings.size();
	for (size_t i = 0; i < count; i++)
	{
		if (boundings[i]->GetEnable() == false)
			continue;

		if (boundings[i]->GetBoundingProp() == BoundingProp::Weapon)
			weapons.push_back(boundings[i]);
		else if (boundings[i]->GetBoundingProp() == BoundingProp::Armor)
			armors.push_back(boundings[i]);
	}
}

void BoundingCollision::CollisionCheck(void)
{
	size_t weaponCount = weapons.size();
	size_t armorCount  = armors.size();


	for (size_t i = 0; i < weaponCount; i++)
	{
		for (size_t j = 0; j < armorCount; j++)
		{
			if (CollisionCheck(weapons[i], armors[j]) == true)
			{
				GameUnit* attacker = dynamic_cast<GameUnit*>(weapons[i]->GetMyModel());
				GameUnit* victim = dynamic_cast<GameUnit*>(armors[j]->GetMyModel());

				weapons[i]->AddAttackList(victim);

				attacker->ActionHit(victim);
				victim->ActionDamage(attacker);

				float c1 = Math::Random(0.0f, 1.0f);
				float c2 = Math::Random(0.0f, 1.0f);
				float c3 = Math::Random(0.0f, 1.0f);
				armors[j]->SetColor(D3DXCOLOR(c1, c2, c3, 1.0f));
			}
		}
	}

	weapons.clear();
	armors.clear();
}

bool BoundingCollision::CollisionCheck(Bounding * weapon, Bounding * armor)
{
	if (weapon->GetMyModel() == armor->GetMyModel())
		return false;
	if (weapon->FindAttackList(armor->GetMyModel()) == true)
		return false;

	BoundingType weaponType = weapon->GetBoundingType();
	BoundingType armorType = armor->GetBoundingType();

	if (weaponType == BoundingType::Capsule)
	{
		if (armorType == BoundingType::Capsule)
		{
			BoundingCapsule* c1 = dynamic_cast<BoundingCapsule*>(weapon);
			BoundingCapsule* c2 = dynamic_cast<BoundingCapsule*>(armor);
			
			return BoundingCapsule::isCollision(c1, c2);
		}
	}

	return false;
}
