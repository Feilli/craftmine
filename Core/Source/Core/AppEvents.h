#pragma once

#include "Core/Event.h"

#include <glm/glm.hpp>

#include <format>

namespace Core {

    class PositionUpdatedEvent : public Event {
    public:
        PositionUpdatedEvent(glm::vec3 position) 
            : m_Position(position) {}

        inline glm::vec3 GetPosition() const { return m_Position; }

        std::string ToString() const override {
            return std::format("PositionUpdatedEvent: x:{}, y:{}, z:{}", m_Position.x, m_Position.y, m_Position.z);
        }

        EVENT_CLASS_TYPE(PositionUpdated)
    private:
        glm::vec3 m_Position;
    };

    class TimeUpdatedEvent : public Event {
    public:
        TimeUpdatedEvent(float dayTime, float dayDuration)
            : m_DayTime(dayTime), m_DayDuration(dayDuration) {}

        inline float GetDayTime() const { return m_DayTime; }
        inline float GetDayDuration() const { return m_DayDuration; }

        std::string ToString() const override {
            return std::format("TimeUpdatedEvent: {}, {}", m_DayTime, m_DayDuration);
        }

        EVENT_CLASS_TYPE(TimeUpdated)
    private:
        float m_DayTime = 0.0f;
        float m_DayDuration = 0.0f;
    };

    class ChunksGeneratedEvent : public Event {
    public:
        ChunksGeneratedEvent(int chunks, float time)
            : m_Chunks(chunks), m_Time(time) {}

        inline int GetChunks() const { return m_Chunks; }
        inline float GetTime() const { return m_Time; }

        std::string ToString() const override {
            return std::format("ChunksGeneratedEvent: {} in {} secodns", m_Chunks, m_Time);
        }

        EVENT_CLASS_TYPE(ChunksGenerated)
    private:
        int m_Chunks = 0;
        float m_Time = 0.0f;
    };

    class SelectedItemUpdatedEvent : public Event {
    public:
        SelectedItemUpdatedEvent(int item)
            : m_SelectedItem(item) {}

        inline int GetSelectedItem() const { return m_SelectedItem; }

        std::string ToString() const override {
            return std::format("SelectedItemUpdatedEvent: {}", m_SelectedItem);
        }

        EVENT_CLASS_TYPE(SelectedItemUpdated)
    private:
        int m_SelectedItem = 0;
    };

}