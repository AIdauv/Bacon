#pragma once

#include "Bacon/Core/Base.h"
#include "Bacon/Core/Log.h"
#include "Bacon/Scene/Scene.h"
#include "Bacon/Scene/Entity.h"

namespace Bacon {

    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& scene);

        void SetContext(const Ref<Scene>& scene);

        void OnImGuiRender();
    private:
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);
    private:
        Ref<Scene> m_Context;
        Entity m_SelectionContext;
    };

}