#include <algorithm>
#include <forward_list>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <queue>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>

using namespace std;

template < typename TTask >
class ManagedQueue
{
//typedefs
public:
    typedef typename queue< TTask > TaskQueue;
    typedef unordered_map< int, int > UserMap;
    typedef forward_list< int > TaskSelections;
    typedef unordered_map< int, TaskQueue > TaskQueues;
//Fields
public:
    UserMap Users;
private:
    TaskQueues tasks;   //每个用户拥有一个TaskQueue
    TaskSelections task_queues; //内含大量UID，其数量和用户配额相同
    mt19937 rng;    //随机数发生器
//Methods
private:
    //向备选队列集合中随机插入用户ID，以便Dequeue时能按配额均匀取出任务
    void RandomInsert(int user_id, int count)
    {
        uniform_int_distribution<int> dist(0, 2);
        if (task_queues.empty())
        {
            for (int i = 0; i < count; i++)
            {
                task_queues.push_front(user_id);
            }
        }
        else
        {
            while (count > 0)
            {
                for (auto i = task_queues.begin(); i != task_queues.end(); i++)
                {
                    if (dist(rng) == 1)
                    {
                        task_queues.insert_after(i, user_id);
                        count--;
                    }
                }
            }
        }
    }
public:
    //UserMap: UID -> 配额
    ManagedQueue(const UserMap& user_table)
        : Users(user_table), rng(random_device()())
    {
        //为每一个用户分配一个任务队列
        for (auto& p : user_table)
        {
            tasks.emplace(p.first, queue< TTask >());
        }
    }
    //如果添加任务前用户任务队列为空，就向备选队列集合中添加与配额相当数量的UID
    void Enqueue(int user_id, const TTask& task)
    {
        auto& q = tasks[user_id];
        bool insert_new = q.empty();
        q.push(task);
        if (insert_new)
        {
            RandomInsert(user_id, Users[user_id]);
        }
    }
    //在备选队列中随机抽一个UID，并根据UID找到对应的任务队列，从任务队列中取出任务
    //如果任务队列中没有任务，那么从备选队列集合中删除对应的UID
    TTask Dequeue()
    {
        uniform_int_distribution<int> dist(0, distance(task_queues.begin(), task_queues.end()) - 1);
        auto it = task_queues.begin();
        advance(it, dist(rng));
        auto& q = tasks[*it];
        auto ret = q.front();
        q.pop();
        if (q.empty())
        {
            task_queues.remove_if([&](int user_id) { return tasks[user_id].empty(); });
        }
        return ret;
    }
};

int main()
{
    unordered_map<int, int> user = { {1001, 10}, {1002, 20}, {1003, 40}, {1004, 30} };
    ManagedQueue<string> q(user);
    for (int i = 0; i < 1000; i++)
    {
        q.Enqueue(1001 + i % 4, string("Task ") + to_string(i % 4 + 1));
    }
    
    for (int i = 0; i < 400; i++)
    {
        cout << q.Dequeue() << "\t";
    }
    return 0;
}