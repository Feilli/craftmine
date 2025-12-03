#include "Core/Application.h"

#include "AppLayer.h"
#include "HUDLayer.h"

int main() {
    Core::ApplicationParams appParams;
    appParams.Name = "CraftMine";
    appParams.WindowParams.Width = 1280;
    appParams.WindowParams.Height = 720;

    Core::Application app(appParams);
    app.PushLayer<AppLayer>();
    app.PushLayer<HUDLayer>();
    app.Run();

    return 0;
}