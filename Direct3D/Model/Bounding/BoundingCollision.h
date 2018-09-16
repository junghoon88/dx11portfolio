#pragma once

class Bounding;

#define gBoundingCollision BoundingCollision::Get()

class BoundingCollision
{
public:
	inline static BoundingCollision* Get(void) { return instance; }
	inline static void Create(void) { instance = new BoundingCollision(); }
	inline static void Delete(void) { BoundingCollision(instance); }

private:
	BoundingCollision();
	~BoundingCollision();
	static BoundingCollision* instance;

public:
	void AddBoundings(vector<Bounding*>& vec);

	void CollisionCheck(void);

private:
	bool CollisionCheck(Bounding* weapon, Bounding* armor);


private:
	vector<Bounding*> weapons;
	vector<Bounding*> armors;
};



