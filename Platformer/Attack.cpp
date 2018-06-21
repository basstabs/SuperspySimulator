//Includes

//Custom Includes
#include "Helpers.h"
#include "Level.h"
#include "Attack.h"

namespace Platformer
{

	Vector2 UpdateBullet(Bullet* bullet, float deltaTime)
	{

		return bullet->RawVelocity() * deltaTime;

	}

	HitComponent::HitComponent(Attack* a)
	{

		this->parent = a;

	}

	int HitComponent::Damage()
	{

		return -1;

	}

	DamageComponent::DamageComponent(Attack* a, int d) : HitComponent(a)
	{

		this->damage = d;

	}

	void DamageComponent::Hit(GameEntity* entity)
	{

		entity->Hurt(this->parent);

	}

	int DamageComponent::Damage()
	{

		return this->damage;

	}

	BindingComponent::BindingComponent(Attack* a) : HitComponent(a)
	{

	}

	void BindingComponent::Hit(GameEntity* entity)
	{

		entity->RequestEvent("Bind");

		if (this->parent->Type() == "Melee")
		{

			this->parent->Parent()->SetActive(false);

		}

	}

	Attack::Attack() : GameObject()
	{

		this->component = new DamageComponent(this, 0);

		this->parent = NULL;

	}

	Attack::Attack(GameEntity* p, Level* l, int d, float x, float y, float w, float h) : GameObject(x, y, w, h)
	{

		if (d >= 0)
		{

			this->component = new DamageComponent(this, d);

		}
		else
		{

			this->component = new BindingComponent(this);

		}

		this->parent = p;

		this->level = l;

	}

	Attack::~Attack()
	{

		if (this->component)
		{

			delete this->component;
			this->component = NULL;

		}

	}

	void Attack::LoadSpritesheet(std::string fileName, SDL_Texture* surface)
	{

		this->sheet.LoadSpritesheet(fileName);
		this->sheet.SetSurface(surface);

	}

	void Attack::Update(std::vector<GameEntity*> entities, float deltaTime)
	{

		this->CheckForHits(entities, deltaTime);

	}

	bool Attack::RemoveCondition()
	{

		return false;

	}

	void Attack::Render(Vector2 world, float deltaTime)
	{

		if (this->sheet.Surface())
		{

			this->sheet.Render(Vector2(this->X() + this->offset.X(), this->Y() + this->offset.Y()) - world, (this->rawVelocity.X() < 0));

		}

	}

	void Attack::CheckForHits(std::vector<GameEntity*> entities, float deltaTime)
	{

		for (int i = 0; i < entities.size(); i++)
		{

			bool x = this->Active();
			if (this->Active() && entities[i]->Active() && (!this->parent || this->parent->Type()[0] != entities[i]->Type()[0]) && this->Intersects(entities[i]) && GetTime() - entities[i]->LastHit() > entities[i]->HitInvulnerability())
			{

				this->Hit(entities[i]);

			}

		}

	}

	void Attack::Hit(GameEntity* entity)
	{

		this->component->Hit(entity);

		if (this->RemoveCondition())
		{

			this->Remove();

		}

	}

	void Attack::Remove()
	{

		this->remove = true;

	}

	bool Attack::ShouldRemove(Vector2 world)
	{

		return this->remove;

	}

	bool Attack::Active()
	{

		return true;

	}

	Spritesheet* Attack::Sheet()
	{

		return &this->sheet;

	}

	void Attack::SetParent(GameEntity* p)
	{

		this->parent = p;

	}

	void Attack::SetLevel(Level* l)
	{

		this->level = l;

	}

	int Attack::Damage()
	{

		if (this->component)
		{

			return this->component->Damage();

		}

		return 0;

	}

	GameEntity* Attack::Parent()
	{

		return this->parent;

	}

	std::string Attack::Type()
	{

		return "Attack";

	}

	BodyAttack::BodyAttack() : Attack()
	{

	}

	BodyAttack::BodyAttack(GameEntity* p, Level* l, int d, float x, float y, float w, float h) : Attack(p, l, d, x, y, w, h)
	{

	}

	BodyAttack::~BodyAttack()
	{

	}

	void BodyAttack::Update(std::vector<GameEntity*> entities, float deltaTime)
	{

		if (this->parent)
		{

			this->SetX(this->parent->X());
			this->SetY(this->parent->Y());
			this->SetWidth(this->parent->Width());
			this->SetHeight(this->parent->Height());

			this->CheckForHits(entities, deltaTime);

			this->sheet.Update(deltaTime);

		}
		else
		{

			this->Remove();

		}

	}

	bool BodyAttack::RemoveCondition()
	{

		return !this->parent->Active();

	}

	bool BodyAttack::Active()
	{

		if (this->parent && this->parent->Active())
		{

			return true;

		}

		this->Remove();
		
		return false;

	}

	std::string BodyAttack::Type()
	{

		return "Body";

	}

	MeleeAttack::MeleeAttack() : Attack()
	{

	}

	MeleeAttack::MeleeAttack(GameEntity* p, Level* l, int d, float x, float y, float w, float h, float lOX, float lOY, float rOX, float rOY, int index) : Attack(p, l, d, x, y, w, h)
	{

		this->lOffset = Vector2(lOX, lOY);
		this->rOffset = Vector2(rOX, rOY);

		this->animIndex = index;
	
		this->damage = d;

	}

	MeleeAttack::~MeleeAttack()
	{

	}

	void MeleeAttack::Update(std::vector<GameEntity*> entities, float deltaTime)
	{

		this->SetX(this->parent->X() + this->Offset().X());
		this->SetY(this->parent->Y() + this->Offset().Y());

		this->CheckForHits(entities, deltaTime);

		this->sheet.Update(deltaTime);

	}

	bool MeleeAttack::RemoveCondition()
	{

		return !this->Active();

	}

	bool MeleeAttack::ShouldRemove(Vector2 world)
	{

		return this->RemoveCondition();

	}

	bool MeleeAttack::Active()
	{

		return this->parent && this->parent->Sheet()->CurrentAnimation() == this->animIndex;

	}

	Vector2 MeleeAttack::Offset()
	{

		if (this->parent->Direction() < 0)
		{

			return Vector2(this->parent->Width() + lOffset.X() - this->Width(), this->lOffset.Y());

		}

		return Vector2(this->rOffset.X(), this->rOffset.Y());

	}

	MeleeAttack* MeleeAttack::Clone(GameEntity* p)
	{

		return new MeleeAttack(p, this->parent->GetLevel(), this->damage, this->X(), this->Y(), this->Width(), this->Height(), this->lOffset.X(), this->lOffset.Y(), this->rOffset.X(), this->rOffset.Y(), this->animIndex);

	}

	std::string MeleeAttack::Type()
	{

		return "Melee";

	}

	Bullet::Bullet() : Attack()
	{

	}

	Bullet::Bullet(GameEntity* p, Level* l, int hit, int d, float x, float y, float w, float h) : Attack(p, l, d, x, y, w, h)
	{

		this->hits = hit;

		this->offset = y;

	}

	Bullet::~Bullet()
	{

	}

	void Bullet::Update(std::vector<GameEntity*> entities, float deltaTime)
	{

		this->Move(UpdateBullet(this, deltaTime));

		this->CheckForHits(entities, deltaTime);

		if (!this->level->Intersects(this) || this->level->Collision()->Intersects(this))
		{

			this->Remove();

		}

		this->sheet.Update(deltaTime);

	}

	bool Bullet::RemoveCondition()
	{
		
		this->hits--;

		return (this->hits == 0);

	}

	bool Bullet::ShouldRemove(Vector2 world)
	{

		//May use this later, will require re-design
		AABB viewport(world.X(), world.Y(), PLATFORMER_TARGET_WIDTH, PLATFORMER_TARGET_HEIGHT);

		return this->Attack::ShouldRemove(world);

	}

	void Bullet::Render(Vector2 world, float deltaTime)
	{

		this->Attack::Render(world + Vector2(0, this->offset), deltaTime);

	}

	std::string Bullet::Type()
	{

		return this->type;

	}

	void Bullet::SetType(std::string t)
	{

		this->type = t;

	}

	Bullet* Bullet::Clone(Vector2 pos, int direction)
	{

		if (this->parent)
		{

			Bullet* bullet = new Bullet(this->parent, this->level, this->hits, this->Damage(), pos.X() + this->X(), pos.Y() + this->Y(), this->Width(), this->Height());
			bullet->SetRawVelocity(this->RawVelocity());

			bullet->offset = this->offset;

			Spritesheet* sheet = bullet->Sheet();

			bullet->Sheet()->Clone(&this->sheet);
			bullet->Sheet()->SetSurface(this->sheet.Surface());

			bullet->SetType(this->Type());

			return bullet;

		}

		return NULL;

	}

	bool Bullet::Active()
	{

		return this->hits != 0;

	}

}