#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCParticleSystemQuad.hpp>

#include <atomic>

using namespace geode::prelude;

namespace {
    // These cached flags keep settings lookups out of the per-frame draw hook.
    std::atomic_bool g_potatoMode{false};
    std::atomic_bool g_reduceDenseParticles{false};

    void refreshParticleSettings() {
        auto mod = Mod::get();
        g_potatoMode.store(
            mod->getSettingValue<bool>("potato-mode"),
            std::memory_order_relaxed
        );
        g_reduceDenseParticles.store(
            mod->getSettingValue<bool>("reduce-particles"),
            std::memory_order_relaxed
        );
    }
}

$on_mod(Loaded) {
    refreshParticleSettings();

    listenForSettingChanges<bool>("potato-mode", [](bool) {
        refreshParticleSettings();
    });

    listenForSettingChanges<bool>("reduce-particles", [](bool) {
        refreshParticleSettings();
    });
}

// Rendering optimization: skip particle-system draw calls when requested.
// This intentionally does not stop particle simulation, which can still use CPU.
class $modify(GDOptimizerParticleSystemQuad, CCParticleSystemQuad) {
    void draw() {
        if (g_potatoMode.load(std::memory_order_relaxed)) {
            return;
        }

        // Less aggressive than Potato Mode: hide only currently dense systems.
        if (
            g_reduceDenseParticles.load(std::memory_order_relaxed) &&
            this->getParticleCount() >= 12
        ) {
            return;
        }

        CCParticleSystemQuad::draw();
    }
};

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
