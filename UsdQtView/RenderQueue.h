#pragma once
#include "NodeRenderer.h"
#include "gl.h"


class RenderQueue final {
public:
    void add(NodeRenderer* pRenderer, uint priority = 0) {
        items_.emplace_back(pRenderer, priority);
    }

    void clear() { items_.clear(); }

    void sort() {
        std::sort(std::begin(items_), std::end(items_), [](const auto& lhs, const auto& rhs) {
            return lhs.hash() < rhs.hash();
        });
    }

    void render(std::function<void(MaterialVariation*)> onSwitchMaterial) {
        auto material = items_[0].pRenderer->material();
        onSwitchMaterial(&material);

        for (auto& item : items_) {
            if (item.pRenderer->material().hash() != material.hash()) {
                onSwitchMaterial(&material);
            }
            item.pRenderer->render();
        }
    }

private:
    struct QueueItem {
        NodeRenderer* pRenderer;
        uint priority;
        
        QueueItem(NodeRenderer* pRenderer, uint priority)
            : pRenderer(pRenderer), priority(priority)
        { }

        uint64_t hash() const {
            return (static_cast<uint64_t>(pRenderer->material().hash()) << 32) | priority;
        }
    };

    std::vector<QueueItem> items_;
};
