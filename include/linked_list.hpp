#pragma once

template <class T>
class LinkedList
{
public:
    struct Node
    {
        T data;
        Node* next = nullptr;
    };

    LinkedList()
    : head(nullptr)
    {
    }

    LinkedList<T>::Node* head;

    auto push_front(const T& value)
    {
        auto new_node = new Node{value};
        new_node->next = head;
        head = new_node;
        return head;
    }

    auto push_back(const T& value)
    {
        auto tmp = head;
        while (tmp->next)
        {
            tmp = tmp->next;
        }
        tmp->next = new Node{value};
        return tmp->next;
    }

    auto find_value(const T& value)
    {
        auto tmp = head;
        while (tmp)
        {
            if (tmp->data == value)
            {
                return tmp;
            }
            tmp = tmp->next;
        }
        return nullptr;
    }

    /** \brief Move a boid from source list to target list without
     * reallocating any memory.
     */
    static void swap(LinkedList<T>& source_list, LinkedList<T>& target_list, Node* boid)
    {
        if (source_list.head == boid)
        {
            source_list.head = source_list.head->next;
            return;
        }

        auto prev = find_previous_static(source_list, boid);
        prev->next = boid->next;
        boid->next = target_list.head;
        target_list.head = boid;
    }

    auto insert_after(LinkedList<T>::Node* after_me, const T& newvalue)
    {
        auto new_node = new Node{newvalue};
        new_node->next = after_me->next;
        after_me->next = new_node;
        return new_node;
    }

    void remove(LinkedList<T>::Node* node)
    {
        if (node == head)
        {
            auto tmp = head->next;
            delete head;
            head = tmp;
        }
        else
        {
            auto prev = head;
            while (prev->next != node)
            {
                if (prev->next == nullptr)
                {
                    return;
                }
                prev = prev->next;
            }
            prev->next = node->next;
            delete node;
        }
    }

    static auto find_previous_static(LinkedList<T>& list, LinkedList<T>::Node* node_to_find)
    {
        auto tmp = list.head;
        while (tmp->next != node_to_find)
        {
            tmp = tmp->next;
        }
        return tmp;
    }

    auto find_previous(LinkedList<T>::Node* node)
    {
        auto tmp = head;
        while (tmp->next != node)
        {
            tmp = tmp->next;
        }
        return tmp;
    }

    ~LinkedList()
    {
        while (head)
        {
            auto tmp = head->next;
            delete head;
            head = tmp;
        }
    }
};
