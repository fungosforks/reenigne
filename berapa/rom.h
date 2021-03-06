template<class T> class ROMTemplate : public ISA8BitComponentTemplate<T>
{
public:
    ROMTemplate(Simulator* simulator, int mask, int address, String fileName,
        int offset)
    {
        _mask = mask | 0xc0000000;
        _start = address;
        String data = File(romData.file(),
            simulator->config()->file().parent(), true).contents();
        int length = ((_start | ~_mask) & 0xfffff) + 1 - _start;
        _data.allocate(length);
        for (int i = 0; i < length; ++i)
            _data[i] = data[i + offset];
    }
    void setAddress(UInt32 address)
    {
        _address = address & 0xfffff & ~_mask;
        this->_active = ((address & _mask) == _start);
    }
    void read() { this->set(_data[_address & ~_mask]); }
    UInt8 memory(UInt32 address)
    {
        if ((address & _mask) == _start)
            return _data[address & ~_mask];
        return 0xff;
    }
    String save() const
    {
        return String("{ active: ") + String::Boolean(this->_active) +
            ", address: " + hex(_address, 5) + "}\n";
    }
    ::Type persistenceType() const
    {
        List<StructuredType::Member> members;
        members.add(StructuredType::Member("active", false));
        members.add(StructuredType::Member("address", 0));
        return StructuredType("ROM", members);
    }
    void load(const TypedValue& value)
    {
        auto members = value.value<Value<HashTable<Identifier, TypedValue>>>();
        this->_active = (*members)["active"].value<bool>();
        _address = (*members)["address"].value<int>();
    }

    class Type : public ISA8BitComponent::Type
    {
    public:
        Type(Simulator* simulator)
          : Component::Type(new Implementation(simulator)) { }
    private:
        class Implementation : public ISA8BitComponent::Type::Implementation
        {
        public:
            Implementation(Simulator* simulator)
              : Component::Type::Implementation(simulator)
            {
                List<StructuredType::Member> members;
                members.add(StructuredType::Member("mask", IntegerType()));
                members.add(StructuredType::Member("address", IntegerType()));
                members.add(StructuredType::Member("fileName", StringType()));
                members.add(StructuredType::Member("fileOffset",
                    TypedValue(IntegerType(), 0)));
                _structuredType = StructuredType(toString(), members);
            }
            String toString() const { return "ROM"; }
            TypedValue tryConvert(const TypedValue& value, String* why) const
            {
                TypedValue stv = value.type().tryConvertTo(_structuredType,
                    value, why);
                if (!stv.valid())
                    return stv;
                auto romMembers =
                    stv.value<Value<HashTable<Identifier, TypedValue>>>();
                int mask = (*romMembers)["mask"].value<int>();
                int address = (*romMembers)["address"].value<int>();
                String file = (*romMembers)["fileName"].value<String>();
                int offset = (*romMembers)["fileOffset"].value<int>();
                ROM* rom = new ROM(_simulator, mask, address, file, offset);
                _simulator->addComponent(rom);
                return TypedValue(this, rom, value.span());
            }
        private:
            StructuredType _structuredType;
        };
    };
private:
    int _mask;
    int _start;
    int _address;
    Array<UInt8> _data;
};
