#include <optional>
#include <string>
#include "digitalcurling/plugins/plugin_error.hpp"
#include "digitalcurling/plugins/native_loader.hpp"

namespace digitalcurling::plugins {

ModulePtr LoadSharedLibrary(const std::filesystem::path& path) {
    std::string errorHeader = "\"" + path.string() + "\": ";
    std::string errorMessage = "Failed to load plugin. Unable to retrieve detailed error information.";

#if defined(_WIN32)
    ModulePtr handle = LoadLibraryW(path.c_str());
    if (handle) return handle;

    DWORD code = GetLastError();
    if (code == 0)
        throw plugin_error { DIGITALCURLING_ERR_FAILED_TO_LOAD_PLUGIN, errorHeader + errorMessage };

    LPWSTR buffer = nullptr;
    size_t size = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
        code, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPWSTR)&buffer, 0, NULL);

    if (size > 0) {
        auto message = [buffer]() -> std::optional<std::string> {
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, NULL, 0, NULL, NULL);
            if (size_needed > 0) {
                std::string message;
                message.resize(size_needed);
                if (WideCharToMultiByte(CP_UTF8, 0, buffer, -1, &message[0], size_needed, NULL, NULL) > 0) {
                    message.resize(size_needed - 1);
                    return message;
                }
            }
            return std::nullopt;
        }();
        LocalFree(buffer);

        if (message.has_value()) {
            std::string msg = message.value();
            while (!msg.empty() && (msg.back() == '\r' || msg.back() == '\n')) {
                msg.pop_back();
            }
            throw plugin_error { DIGITALCURLING_ERR_FAILED_TO_LOAD_PLUGIN, errorHeader + msg };
        }
    }
    throw plugin_error {
        DIGITALCURLING_ERR_FAILED_TO_LOAD_PLUGIN,
        errorHeader + errorMessage + " (error code " + std::to_string(code) + ")"
    };
#else
    ModulePtr handle = dlopen(path.c_str(), RTLD_LAZY);
    if (handle) return handle;

    const char* error = dlerror();
    throw plugin_error {
        DIGITALCURLING_ERR_FAILED_TO_LOAD_PLUGIN,
        errorHeader + (error ? std::string(error) : errorMessage)
    };
#endif
}

PluginExportedFunctions GetPluginExportedFunctions(ModulePtr module) {
    constexpr const char* get_plugin_info_name = "dc_get_plugin_info";
    constexpr const char* get_plugin_api_name = "dc_get_plugin_api";

#ifdef _WIN32
    auto get_info_func_ptr = GetProcAddress(module, get_plugin_info_name);
    auto get_api_func_ptr = GetProcAddress(module, get_plugin_api_name);
#else
    auto get_info_func_ptr = dlsym(module, get_plugin_info_name);
    auto get_api_func_ptr = dlsym(module, get_plugin_api_name);
#endif

    auto funcs = PluginExportedFunctions();
    funcs.get_plugin_info = reinterpret_cast<GetPluginInfoFunc>(get_info_func_ptr);
    funcs.get_plugin_api = reinterpret_cast<GetPluginApiFunc>(get_api_func_ptr);
    return funcs;
}

bool CloseSharedLibrary(ModulePtr module) {
#ifdef _WIN32
    return FreeLibrary(module);
#else
    return dlclose(module) == 0;
#endif
}

} // namespace digitalcurling::plugins
