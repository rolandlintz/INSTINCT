/// @file Pin.hpp
/// @brief Pin class
/// @author T. Topp (thomas@topp.cc)
/// @date 2020-12-14

#pragma once

#include <imgui_node_editor.h>

#include <string>
#include <string_view>
#include <variant>

namespace NAV
{
class Node;

class Pin
{
  public:
    /// @brief Type of the data on the Pin
    struct Type
    {
        /// @brief Type of the data on the Pin
        enum Value : uint8_t
        {
            Flow,     ///< NodeData Trigger
            Bool,     ///< Boolean
            Int,      ///< Integer Number
            Float,    ///< Floating Point Number
            String,   ///< std::string
            Object,   ///< Generic Object
            Matrix,   ///< Matrix Object
            Function, ///< Callback
            Delegate, ///< Reference to the Node object
        };

        Type() = default;

        //NOLINTNEXTLINE(hicpp-explicit-conversions)
        constexpr Type(Value type)
            : value(type) {}

        explicit Type(const std::string& typeString)
        {
            if (typeString == "Flow")
            {
                value = Type::Flow;
            }
            else if (typeString == "Bool")
            {
                value = Type::Bool;
            }
            else if (typeString == "Int")
            {
                value = Type::Int;
            }
            else if (typeString == "Float")
            {
                value = Type::Float;
            }
            else if (typeString == "String")
            {
                value = Type::String;
            }
            else if (typeString == "Object")
            {
                value = Type::Object;
            }
            else if (typeString == "Matrix")
            {
                value = Type::Matrix;
            }
            else if (typeString == "Function")
            {
                value = Type::Function;
            }
            else if (typeString == "Delegate")
            {
                value = Type::Delegate;
            }
        }

        explicit operator Value() const { return value; } // Allow switch(Pin::Value(type)) and comparisons.
        explicit operator bool() = delete;                // Prevent usage: if(fruit)
        Type& operator=(Value v)
        {
            value = v;
            return *this;
        }

        constexpr bool operator==(const Type& other) const { return value == other.value; }
        constexpr bool operator!=(const Type& other) const { return value != other.value; }

        explicit operator std::string() const
        {
            switch (value)
            {
            case Type::Flow:
                return "Flow";
            case Type::Bool:
                return "Bool";
            case Type::Int:
                return "Int";
            case Type::Float:
                return "Float";
            case Type::String:
                return "String";
            case Type::Object:
                return "Object";
            case Type::Matrix:
                return "Matrix";
            case Type::Function:
                return "Function";
            case Type::Delegate:
                return "Delegate";
            }
        }

      private:
        Value value;
    };

    /// Kind of the Pin (Input/Output)
    struct Kind
    {
        /// @brief Kind of the Pin (Input/Output)
        enum Value : uint8_t
        {
            Output,
            Input,
        };

        Kind() = default;

        //NOLINTNEXTLINE(hicpp-explicit-conversions)
        constexpr Kind(Value kind)
            : value(kind) {}

        explicit Kind(const std::string& kindString)
        {
            if (kindString == "Input")
            {
                value = Kind::Input;
            }
            else if (kindString == "Output")
            {
                value = Kind::Output;
            }
        }

        explicit operator Value() const { return value; } // Allow switch(Pin::Value(type)) and comparisons.
        explicit operator bool() = delete;                // Prevent usage: if(fruit)
        Kind& operator=(Value v)
        {
            value = v;
            return *this;
        }

        constexpr bool operator==(const Kind& other) const { return value == other.value; }
        constexpr bool operator!=(const Kind& other) const { return value != other.value; }

        explicit operator std::string() const
        {
            switch (value)
            {
            case Kind::Input:
                return "Input";
            case Kind::Output:
                return "Output";
            }
        }

      private:
        Value value;
    };

    using PinData = std::variant<void*, bool*, int*, float*, double*, std::string*>;

    /// @brief Default constructor
    Pin() = default;
    /// @brief Destructor
    ~Pin() = default;
    /// @brief Copy constructor
    Pin(const Pin&) = default;
    /// @brief Move constructor
    Pin(Pin&&) = default;
    /// @brief Copy assignment operator
    Pin& operator=(const Pin&) = default;
    /// @brief Move assignment operator
    Pin& operator=(Pin&&) = default;

    /// @brief Constructor
    /// @param[in] id Unique Id of the Pin
    /// @param[in] name Name of the Pin
    /// @param[in] type Type of the Pin
    /// @param[in] pinKind Kind of the Pin (Input/Output)
    /// @param[in] parentNode Reference to the parent node
    Pin(ax::NodeEditor::PinId id, const char* name, Type type, Kind kind, Node* parentNode)
        : id(id), name(name), type(type), kind(kind), parentNode(parentNode) {}

    /// @brief Checks if this pin can connect to the provided pin
    /// @param[in] b The pin to create a link to
    /// @return True if it can create a link
    [[nodiscard]] bool canCreateLink(const Pin& b) const;

    /// @brief Get the Icon Color object
    /// @return Color struct
    [[nodiscard]] ImColor getIconColor() const;

    /// @brief Draw the Pin Icon
    /// @param[in] connected Flag if the pin is connected
    /// @param[in] alpha Alpha value of the pin
    void drawPinIcon(bool connected, int alpha) const;

    /// Unique Id of the Pin
    ax::NodeEditor::PinId id;
    /// Name of the Pin
    std::string name;
    /// Type of the Pin
    Type type;
    /// Kind of the Pin (Input/Output)
    Kind kind;
    /// Reference to the parent node
    Node* parentNode = nullptr;
    /// Pointer to data which is transferred over this pin
    PinData data = static_cast<void*>(nullptr);
    /// Unique name which is used for data flows
    std::string_view dataIdentifier;

  private:
    /// Size of the Pin Icons in [px]
    static constexpr int m_PinIconSize = 24;
};

} // namespace NAV