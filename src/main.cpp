
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(GDOptimizerPlayLayer, PlayLayer) {
    struct Fields {
        CCLabelBMFont* fpsLabel = nullptr;
        float elapsed = 0.0f;
        int frames = 0;
    };

    bool init(
        GJGameLevel* level,
        bool useReplay,
        bool dontCreateObjects
    ) {
        if (!PlayLayer::init(
            level, useReplay, dontCreateObjects
        )) {
            return false;
        }

        if (!Mod::get()->getSettingValue<bool>("show-fps")) {
            return true;
        }

        auto size = CCDirector::sharedDirector()->getWinSize();
        auto label = CCLabelBMFont::create(
            "FPS: -- | Frame: -- ms",
            "bigFont.fnt"
        );

        if (label) {
            label->setScale(0.35f);
            label->setAnchorPoint({1.0f, 1.0f});
            label->setPosition({
                size.width - 8.0f,
                size.height - 8.0f
            });
            this->addChild(label, 1000);
            m_fields->fpsLabel = label;
        }

        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        if (!m_fields->fpsLabel || dt <= 0.0f) {
            return;
        }

        m_fields->elapsed += dt;
        ++m_fields->frames;

        if (m_fields->elapsed >= 0.5f) {
            float fps = m_fields->frames / m_fields->elapsed;
            float ms = 1000.0f / fps;

            auto text = fmt::format(
                "FPS: {:.0f} | Frame: {:.2f} ms",
                fps, ms
            );

            m_fields->fpsLabel->setString(text.c_str());
            m_fields->elapsed = 0.0f;
            m_fields->frames = 0;
        }
    }
};
