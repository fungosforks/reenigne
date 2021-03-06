class NMISwitch : public ISA8BitComponent
{
public:
    void setAddress(UInt32 address)
    {
        _active = (address & 0xc00003e0) == 0xc00000a0;
    }
    void write(UInt8 data) { _nmiOn = ((data & 0x80) != 0); }
    String save() const
    {
        return String("{ on: ") + String::Boolean(_nmiOn) + ", active: " +
            String::Boolean(_active) + " }\n";
    }
    ::Type persistenceType() const
    {
        List<StructuredType::Member> members;
        members.add(StructuredType::Member("on", false));
        members.add(StructuredType::Member("active", false));
        return StructuredType("NMISwitch", members);
    }
    void load(const TypedValue& value)
    {
        auto members = value.value<Value<HashTable<Identifier, TypedValue>>>();
        _nmiOn = (*members)["on"].value<bool>();
        _active = (*members)["active"].value<bool>();
    }
    bool nmiOn() const { return _nmiOn; }
    TypedValue getValue(Identifier name)
    {
        if (name.name() == "bus")
            return &_connector;
        return ISA8BitComponent::getValue(name);
    }

    class ValueConnector : public Connector
    {
    public:
        ValueConnector(NMISwitch* component) : _component(component) { }
        void connect(Connector* other)
        {
            // TODO
        }
        Type type() const { return BitOutputConnectorType(); }
    private:
        NMISwitch* _component;
    };

    class Type : public ISA8BitComponent::Type
    {
    public:
        Type(Simulator* simulator)
          : ISA8BitComponent::Type(new Implementation(simulator)) { }
        Type(const Implementation* implementation)
            : ISA8BitComponent::Type(implementation) { }
    private:
        class Implementation : public ISA8BitComponent::Type::Implementation
        {
        public:
            Implementation(Simulator* simulator)
              : ISA8BitComponent::Type::Implementation(simulator) { }
            String toString() const { return "NMISwitch"; }
            TypedValue tryConvert(const TypedValue& value, String* why) const
            {
                TypedValue stv = value.type().tryConvertTo(
                    StructuredType::empty().type(), value, why);
                if (!stv.valid())
                    return stv;

                NMISwitch* nmiSwitch = new NMISwitch(Type(this));
                _simulator->addComponent(nmiSwitch);
                return TypedValue(this, nmiSwitch, value.span());
            }
            bool has(String memberName) const
            {
                if (memberName == "value")
                    return true;
                return ISA8BitComponent::Type::Implementation::has(memberName);
            }

        };
    };
    NMISwitch(Type type) : _type(type) { }

    Component::Type type() const { return _type; }

private:
    bool _nmiOn;
    Type _type;
};
