#include <map>
#include <string>
#include <iostream>

using namespace std;

// Перечислимый тип для статуса задачи
/*enum class TaskStatus {
    NEW,          // новая
    IN_PROGRESS,  // в разработке
    TESTING,      // на тестировании
    DONE          // завершена
};*/

// Объявляем тип-синоним для map<TaskStatus, int>,
// позволяющего хранить количество задач каждого статуса
using TasksInfo = map<TaskStatus, int>;

void removeZeros(TasksInfo& tasksInfo) {
    for (auto it = tasksInfo.begin(); it != tasksInfo.end();) {
        it = it->second == 0 ? tasksInfo.erase(it) : ++it;
    }
}

class TeamTasks {
public:
    const TasksInfo& GetPersonTasksInfo(const string& person) const {
        return mStorage.at(person);
    }

    void AddNewTask(const string& person) {
        ++mStorage[person][TaskStatus::NEW];
    }

    tuple<TasksInfo, TasksInfo> PerformPersonTasks(const string& person, int task_count) {
        auto& tasks = mStorage[person];
        tuple<TasksInfo, TasksInfo> result;
        if (tasks.size() == 0) {
            return result;
        }
        int done = tasks[TaskStatus::DONE];
        for (auto it = tasks.begin(); it != tasks.end() && it->first != TaskStatus::DONE; ++it) {
            if (task_count == 0) {
                get<1>(result)[it->first] = it->second;
                continue;
            }
            int movedTasks = min(it->second, task_count);
            task_count -= movedTasks;
            auto nextStatus = static_cast<TaskStatus>(static_cast<int>(it->first) + 1);
            get<0>(result)[nextStatus] = movedTasks;
            get<1>(result)[it->first] = it->second - movedTasks;
        }
        tasks = get<0>(result);
        for (auto [key, value] : get<1>(result)) {
            tasks[key] += value;
        }
        tasks[TaskStatus::DONE] += done;
        removeZeros(tasks);
        removeZeros(get<0>(result));
        removeZeros(get<1>(result));
        return result;
    }

private:
    map<string, TasksInfo> mStorage;
};

// Принимаем словарь по значению, чтобы иметь возможность
// обращаться к отсутствующим ключам с помощью [] и получать 0,
// не меняя при этом исходный словарь
void PrintTasksInfo(TasksInfo tasks_info) {
    cout << tasks_info[TaskStatus::NEW] << " new tasks" <<
        ", " << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress" <<
        ", " << tasks_info[TaskStatus::TESTING] << " tasks are being tested" <<
        ", " << tasks_info[TaskStatus::DONE] << " tasks are done" << endl;
}

int main() {
    TeamTasks tasks;
    tasks.AddNewTask("Ilia");
    for (int i = 0; i < 3; ++i) {
        tasks.AddNewTask("Ivan");
    }
    cout << "Ilia's tasks: ";
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"));
    cout << "Ivan's tasks: ";
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"));

    TasksInfo updated_tasks, untouched_tasks;

    tie(updated_tasks, untouched_tasks) =
        tasks.PerformPersonTasks("Ivan", 2);
    cout << "Updated Ivan's tasks: ";
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: ";
    PrintTasksInfo(untouched_tasks);


    tie(updated_tasks, untouched_tasks) =
        tasks.PerformPersonTasks("Ivan", 2);
    cout << "Updated Ivan's tasks: ";
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: ";
    PrintTasksInfo(untouched_tasks);

    return 0;
}