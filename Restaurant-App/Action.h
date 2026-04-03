#ifndef ACTION_H_
#define ACTION_H_

class Restaurant;

class Action {
protected:
    int ActionTime;
    Restaurant* pRest;

public:
    Action(int AT, Restaurant* pR) : ActionTime(AT), pRest(pR) {}

    int getActionTime() const { return ActionTime; }

    virtual void Execute() = 0;

    virtual ~Action() {}
};

#endif