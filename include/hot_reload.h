#pragma once

#include <array>
#include <string>
#include <stdexcept>
#include <cassert>

#include <dlfcn.h>
namespace hot_reload {

/// @brief Base class for libraries that can be hot-reloaded
///
/// @tparam E Type of subclass that derives Module
/// @tparam NumSymbols Number of symbols exported by library
///
/// @note Public functions operate on a singleton (class instance)
template<typename E, size_t NumSymbols>
class Module
{
    using SymbolArray = std::array< std::pair<const char*, void*>, NumSymbols>;

private:

    /// Handle to loaded library
    void* m_libHandle;
    /// Maps symbol names to their pointers in memory
    SymbolArray& m_symbols;

public:

    /// Loads library
    static void LoadLibrary() { GetInstance().Load(); }
    /// Reloads library
    static void ReloadLibrary() { GetInstance().Reload(); }
    /// Unloads library
    static void UnloadLibrary() { GetInstance().Unload(); }

    /// Returns instance by reference
    static E& GetInstance()
    {
        static E instance;
        return instance;
    }

    /// Returns path to library binary
    virtual const char* GetPath() const = 0;

protected:

    // Ctor
    Module(SymbolArray& symbols) : m_symbols(symbols) {}

    /// @brief Executes library function
    /// @tparam index Index of function to execute in symbol array
    /// @tparam Ret Type of return value
    /// @tparam Args Types of arguments
    /// @param args Function arguments
    template <size_t Index, typename Ret, typename... Args>
    Ret Execute(Args... args)
    {
        // Prevent index out of bounds at compile-time
        static_assert(Index >= 0 && Index < NumSymbols, "Out of bounds symbol index");
        // Lookup the function address
        auto symbol = m_symbols[Index];
        return reinterpret_cast<Ret(*)(Args...)>(symbol.second)(args...);
    }

    /// @brief Returns pointer to variable
    /// @tparam Index Index of variable in symbol array
    /// @tparam T Type of variable
    template <size_t Index, typename T>
    T* GetVar()
    {
        // Prevent index out of bounds at compile-time
        static_assert(Index >= 0 && Index < NumSymbols, "Out of bounds symbol index");
        // Lookup the variable address
        auto symbol = m_symbols[Index];
        return static_cast<T*>(symbol.second);
    }

private:

    /// @brief Loads library into memory
    void Load()
    {
        // RTLD_NOW All necessary relocations are performed when the object is first loaded
        m_libHandle = dlopen(GetPath(), RTLD_NOW | RTLD_GLOBAL);
        LoadSymbols();
    }

    /// @brief Unloads library from memory
    void Unload()
    {
        dlclose(m_libHandle);
        m_libHandle = nullptr;
        UnloadSymbols();
    }

    /// @brief Reloads library
    void Reload()
    {
        Unload();
        Load();
    }

    /// @brief Loads symbols into memory
    void LoadSymbols()
    {
        for (auto&& symbol : m_symbols)
        {
            symbol.second = dlsym(m_libHandle, symbol.first);
        }
    }

    /// @brief Unloads symbols from memory
    void UnloadSymbols()
    {
        for (auto&& symbol : m_symbols)
        {
            symbol.second = nullptr;
        }
    }
};

}  // namespace hot_reload
