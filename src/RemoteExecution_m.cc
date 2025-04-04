// By B22CS061 & B22CS062
// Generated file, do not edit! Created by opp_msgtool 6.1 from src/RemoteExecution.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "RemoteExecution_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(BaseMessage)

BaseMessage::BaseMessage(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

BaseMessage::BaseMessage(const BaseMessage& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

BaseMessage::~BaseMessage()
{
}

BaseMessage& BaseMessage::operator=(const BaseMessage& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void BaseMessage::copy(const BaseMessage& other)
{
    this->sourceId = other.sourceId;
    this->destinationId = other.destinationId;
    this->timestamp = other.timestamp;
}

void BaseMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->sourceId);
    doParsimPacking(b,this->destinationId);
    doParsimPacking(b,this->timestamp);
}

void BaseMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->sourceId);
    doParsimUnpacking(b,this->destinationId);
    doParsimUnpacking(b,this->timestamp);
}

const char * BaseMessage::getSourceId() const
{
    return this->sourceId.c_str();
}

void BaseMessage::setSourceId(const char * sourceId)
{
    this->sourceId = sourceId;
}

const char * BaseMessage::getDestinationId() const
{
    return this->destinationId.c_str();
}

void BaseMessage::setDestinationId(const char * destinationId)
{
    this->destinationId = destinationId;
}

int BaseMessage::getTimestamp() const
{
    return this->timestamp;
}

void BaseMessage::setTimestamp(int timestamp)
{
    this->timestamp = timestamp;
}

class BaseMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_sourceId,
        FIELD_destinationId,
        FIELD_timestamp,
    };
  public:
    BaseMessageDescriptor();
    virtual ~BaseMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(BaseMessageDescriptor)

BaseMessageDescriptor::BaseMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(BaseMessage)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

BaseMessageDescriptor::~BaseMessageDescriptor()
{
    delete[] propertyNames;
}

bool BaseMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<BaseMessage *>(obj)!=nullptr;
}

const char **BaseMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *BaseMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int BaseMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int BaseMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_sourceId
        FD_ISEDITABLE,    // FIELD_destinationId
        FD_ISEDITABLE,    // FIELD_timestamp
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *BaseMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "sourceId",
        "destinationId",
        "timestamp",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int BaseMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "sourceId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "destinationId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "timestamp") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *BaseMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_sourceId
        "string",    // FIELD_destinationId
        "int",    // FIELD_timestamp
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **BaseMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *BaseMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int BaseMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    BaseMessage *pp = omnetpp::fromAnyPtr<BaseMessage>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void BaseMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    BaseMessage *pp = omnetpp::fromAnyPtr<BaseMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'BaseMessage'", field);
    }
}

const char *BaseMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    BaseMessage *pp = omnetpp::fromAnyPtr<BaseMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string BaseMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    BaseMessage *pp = omnetpp::fromAnyPtr<BaseMessage>(object); (void)pp;
    switch (field) {
        case FIELD_sourceId: return oppstring2string(pp->getSourceId());
        case FIELD_destinationId: return oppstring2string(pp->getDestinationId());
        case FIELD_timestamp: return long2string(pp->getTimestamp());
        default: return "";
    }
}

void BaseMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    BaseMessage *pp = omnetpp::fromAnyPtr<BaseMessage>(object); (void)pp;
    switch (field) {
        case FIELD_sourceId: pp->setSourceId((value)); break;
        case FIELD_destinationId: pp->setDestinationId((value)); break;
        case FIELD_timestamp: pp->setTimestamp(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'BaseMessage'", field);
    }
}

omnetpp::cValue BaseMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    BaseMessage *pp = omnetpp::fromAnyPtr<BaseMessage>(object); (void)pp;
    switch (field) {
        case FIELD_sourceId: return pp->getSourceId();
        case FIELD_destinationId: return pp->getDestinationId();
        case FIELD_timestamp: return pp->getTimestamp();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'BaseMessage' as cValue -- field index out of range?", field);
    }
}

void BaseMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    BaseMessage *pp = omnetpp::fromAnyPtr<BaseMessage>(object); (void)pp;
    switch (field) {
        case FIELD_sourceId: pp->setSourceId(value.stringValue()); break;
        case FIELD_destinationId: pp->setDestinationId(value.stringValue()); break;
        case FIELD_timestamp: pp->setTimestamp(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'BaseMessage'", field);
    }
}

const char *BaseMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr BaseMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    BaseMessage *pp = omnetpp::fromAnyPtr<BaseMessage>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void BaseMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    BaseMessage *pp = omnetpp::fromAnyPtr<BaseMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'BaseMessage'", field);
    }
}

Register_Class(TaskMessage)

TaskMessage::TaskMessage(const char *name, short kind) : ::BaseMessage(name, kind)
{
}

TaskMessage::TaskMessage(const TaskMessage& other) : ::BaseMessage(other)
{
    copy(other);
}

TaskMessage::~TaskMessage()
{
    delete [] this->values;
}

TaskMessage& TaskMessage::operator=(const TaskMessage& other)
{
    if (this == &other) return *this;
    ::BaseMessage::operator=(other);
    copy(other);
    return *this;
}

void TaskMessage::copy(const TaskMessage& other)
{
    this->taskId = other.taskId;
    this->subtaskId = other.subtaskId;
    delete [] this->values;
    this->values = (other.values_arraysize==0) ? nullptr : new int[other.values_arraysize];
    values_arraysize = other.values_arraysize;
    for (size_t i = 0; i < values_arraysize; i++) {
        this->values[i] = other.values[i];
    }
}

void TaskMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::BaseMessage::parsimPack(b);
    doParsimPacking(b,this->taskId);
    doParsimPacking(b,this->subtaskId);
    b->pack(values_arraysize);
    doParsimArrayPacking(b,this->values,values_arraysize);
}

void TaskMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::BaseMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->taskId);
    doParsimUnpacking(b,this->subtaskId);
    delete [] this->values;
    b->unpack(values_arraysize);
    if (values_arraysize == 0) {
        this->values = nullptr;
    } else {
        this->values = new int[values_arraysize];
        doParsimArrayUnpacking(b,this->values,values_arraysize);
    }
}

int TaskMessage::getTaskId() const
{
    return this->taskId;
}

void TaskMessage::setTaskId(int taskId)
{
    this->taskId = taskId;
}

int TaskMessage::getSubtaskId() const
{
    return this->subtaskId;
}

void TaskMessage::setSubtaskId(int subtaskId)
{
    this->subtaskId = subtaskId;
}

size_t TaskMessage::getValuesArraySize() const
{
    return values_arraysize;
}

int TaskMessage::getValues(size_t k) const
{
    if (k >= values_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)values_arraysize, (unsigned long)k);
    return this->values[k];
}

void TaskMessage::setValuesArraySize(size_t newSize)
{
    int *values2 = (newSize==0) ? nullptr : new int[newSize];
    size_t minSize = values_arraysize < newSize ? values_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        values2[i] = this->values[i];
    for (size_t i = minSize; i < newSize; i++)
        values2[i] = 0;
    delete [] this->values;
    this->values = values2;
    values_arraysize = newSize;
}

void TaskMessage::setValues(size_t k, int values)
{
    if (k >= values_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)values_arraysize, (unsigned long)k);
    this->values[k] = values;
}

void TaskMessage::insertValues(size_t k, int values)
{
    if (k > values_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)values_arraysize, (unsigned long)k);
    size_t newSize = values_arraysize + 1;
    int *values2 = new int[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        values2[i] = this->values[i];
    values2[k] = values;
    for (i = k + 1; i < newSize; i++)
        values2[i] = this->values[i-1];
    delete [] this->values;
    this->values = values2;
    values_arraysize = newSize;
}

void TaskMessage::appendValues(int values)
{
    insertValues(values_arraysize, values);
}

void TaskMessage::eraseValues(size_t k)
{
    if (k >= values_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)values_arraysize, (unsigned long)k);
    size_t newSize = values_arraysize - 1;
    int *values2 = (newSize == 0) ? nullptr : new int[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        values2[i] = this->values[i];
    for (i = k; i < newSize; i++)
        values2[i] = this->values[i+1];
    delete [] this->values;
    this->values = values2;
    values_arraysize = newSize;
}

class TaskMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_taskId,
        FIELD_subtaskId,
        FIELD_values,
    };
  public:
    TaskMessageDescriptor();
    virtual ~TaskMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(TaskMessageDescriptor)

TaskMessageDescriptor::TaskMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(TaskMessage)), "BaseMessage")
{
    propertyNames = nullptr;
}

TaskMessageDescriptor::~TaskMessageDescriptor()
{
    delete[] propertyNames;
}

bool TaskMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TaskMessage *>(obj)!=nullptr;
}

const char **TaskMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *TaskMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int TaskMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int TaskMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_taskId
        FD_ISEDITABLE,    // FIELD_subtaskId
        FD_ISARRAY | FD_ISEDITABLE | FD_ISRESIZABLE,    // FIELD_values
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *TaskMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "taskId",
        "subtaskId",
        "values",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int TaskMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "taskId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "subtaskId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "values") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *TaskMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_taskId
        "int",    // FIELD_subtaskId
        "int",    // FIELD_values
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **TaskMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *TaskMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int TaskMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    TaskMessage *pp = omnetpp::fromAnyPtr<TaskMessage>(object); (void)pp;
    switch (field) {
        case FIELD_values: return pp->getValuesArraySize();
        default: return 0;
    }
}

void TaskMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskMessage *pp = omnetpp::fromAnyPtr<TaskMessage>(object); (void)pp;
    switch (field) {
        case FIELD_values: pp->setValuesArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'TaskMessage'", field);
    }
}

const char *TaskMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    TaskMessage *pp = omnetpp::fromAnyPtr<TaskMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TaskMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    TaskMessage *pp = omnetpp::fromAnyPtr<TaskMessage>(object); (void)pp;
    switch (field) {
        case FIELD_taskId: return long2string(pp->getTaskId());
        case FIELD_subtaskId: return long2string(pp->getSubtaskId());
        case FIELD_values: return long2string(pp->getValues(i));
        default: return "";
    }
}

void TaskMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskMessage *pp = omnetpp::fromAnyPtr<TaskMessage>(object); (void)pp;
    switch (field) {
        case FIELD_taskId: pp->setTaskId(string2long(value)); break;
        case FIELD_subtaskId: pp->setSubtaskId(string2long(value)); break;
        case FIELD_values: pp->setValues(i,string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TaskMessage'", field);
    }
}

omnetpp::cValue TaskMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    TaskMessage *pp = omnetpp::fromAnyPtr<TaskMessage>(object); (void)pp;
    switch (field) {
        case FIELD_taskId: return pp->getTaskId();
        case FIELD_subtaskId: return pp->getSubtaskId();
        case FIELD_values: return pp->getValues(i);
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'TaskMessage' as cValue -- field index out of range?", field);
    }
}

void TaskMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskMessage *pp = omnetpp::fromAnyPtr<TaskMessage>(object); (void)pp;
    switch (field) {
        case FIELD_taskId: pp->setTaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_subtaskId: pp->setSubtaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_values: pp->setValues(i,omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TaskMessage'", field);
    }
}

const char *TaskMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr TaskMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    TaskMessage *pp = omnetpp::fromAnyPtr<TaskMessage>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void TaskMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskMessage *pp = omnetpp::fromAnyPtr<TaskMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TaskMessage'", field);
    }
}

Register_Class(ResultMessage)

ResultMessage::ResultMessage(const char *name, short kind) : ::BaseMessage(name, kind)
{
}

ResultMessage::ResultMessage(const ResultMessage& other) : ::BaseMessage(other)
{
    copy(other);
}

ResultMessage::~ResultMessage()
{
}

ResultMessage& ResultMessage::operator=(const ResultMessage& other)
{
    if (this == &other) return *this;
    ::BaseMessage::operator=(other);
    copy(other);
    return *this;
}

void ResultMessage::copy(const ResultMessage& other)
{
    this->taskId = other.taskId;
    this->subtaskId = other.subtaskId;
    this->result = other.result;
    this->isValid_ = other.isValid_;
}

void ResultMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::BaseMessage::parsimPack(b);
    doParsimPacking(b,this->taskId);
    doParsimPacking(b,this->subtaskId);
    doParsimPacking(b,this->result);
    doParsimPacking(b,this->isValid_);
}

void ResultMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::BaseMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->taskId);
    doParsimUnpacking(b,this->subtaskId);
    doParsimUnpacking(b,this->result);
    doParsimUnpacking(b,this->isValid_);
}

int ResultMessage::getTaskId() const
{
    return this->taskId;
}

void ResultMessage::setTaskId(int taskId)
{
    this->taskId = taskId;
}

int ResultMessage::getSubtaskId() const
{
    return this->subtaskId;
}

void ResultMessage::setSubtaskId(int subtaskId)
{
    this->subtaskId = subtaskId;
}

int ResultMessage::getResult() const
{
    return this->result;
}

void ResultMessage::setResult(int result)
{
    this->result = result;
}

bool ResultMessage::isValid() const
{
    return this->isValid_;
}

void ResultMessage::setIsValid(bool isValid)
{
    this->isValid_ = isValid;
}

class ResultMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_taskId,
        FIELD_subtaskId,
        FIELD_result,
        FIELD_isValid,
    };
  public:
    ResultMessageDescriptor();
    virtual ~ResultMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(ResultMessageDescriptor)

ResultMessageDescriptor::ResultMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(ResultMessage)), "BaseMessage")
{
    propertyNames = nullptr;
}

ResultMessageDescriptor::~ResultMessageDescriptor()
{
    delete[] propertyNames;
}

bool ResultMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ResultMessage *>(obj)!=nullptr;
}

const char **ResultMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ResultMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ResultMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 4+base->getFieldCount() : 4;
}

unsigned int ResultMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_taskId
        FD_ISEDITABLE,    // FIELD_subtaskId
        FD_ISEDITABLE,    // FIELD_result
        FD_ISEDITABLE,    // FIELD_isValid
    };
    return (field >= 0 && field < 4) ? fieldTypeFlags[field] : 0;
}

const char *ResultMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "taskId",
        "subtaskId",
        "result",
        "isValid",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int ResultMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "taskId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "subtaskId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "result") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "isValid") == 0) return baseIndex + 3;
    return base ? base->findField(fieldName) : -1;
}

const char *ResultMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_taskId
        "int",    // FIELD_subtaskId
        "int",    // FIELD_result
        "bool",    // FIELD_isValid
    };
    return (field >= 0 && field < 4) ? fieldTypeStrings[field] : nullptr;
}

const char **ResultMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ResultMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ResultMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ResultMessage *pp = omnetpp::fromAnyPtr<ResultMessage>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void ResultMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ResultMessage *pp = omnetpp::fromAnyPtr<ResultMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ResultMessage'", field);
    }
}

const char *ResultMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ResultMessage *pp = omnetpp::fromAnyPtr<ResultMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ResultMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ResultMessage *pp = omnetpp::fromAnyPtr<ResultMessage>(object); (void)pp;
    switch (field) {
        case FIELD_taskId: return long2string(pp->getTaskId());
        case FIELD_subtaskId: return long2string(pp->getSubtaskId());
        case FIELD_result: return long2string(pp->getResult());
        case FIELD_isValid: return bool2string(pp->isValid());
        default: return "";
    }
}

void ResultMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ResultMessage *pp = omnetpp::fromAnyPtr<ResultMessage>(object); (void)pp;
    switch (field) {
        case FIELD_taskId: pp->setTaskId(string2long(value)); break;
        case FIELD_subtaskId: pp->setSubtaskId(string2long(value)); break;
        case FIELD_result: pp->setResult(string2long(value)); break;
        case FIELD_isValid: pp->setIsValid(string2bool(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ResultMessage'", field);
    }
}

omnetpp::cValue ResultMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ResultMessage *pp = omnetpp::fromAnyPtr<ResultMessage>(object); (void)pp;
    switch (field) {
        case FIELD_taskId: return pp->getTaskId();
        case FIELD_subtaskId: return pp->getSubtaskId();
        case FIELD_result: return pp->getResult();
        case FIELD_isValid: return pp->isValid();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ResultMessage' as cValue -- field index out of range?", field);
    }
}

void ResultMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ResultMessage *pp = omnetpp::fromAnyPtr<ResultMessage>(object); (void)pp;
    switch (field) {
        case FIELD_taskId: pp->setTaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_subtaskId: pp->setSubtaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_result: pp->setResult(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_isValid: pp->setIsValid(value.boolValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ResultMessage'", field);
    }
}

const char *ResultMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr ResultMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ResultMessage *pp = omnetpp::fromAnyPtr<ResultMessage>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ResultMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ResultMessage *pp = omnetpp::fromAnyPtr<ResultMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ResultMessage'", field);
    }
}

Register_Class(GossipMessage)

GossipMessage::GossipMessage(const char *name, short kind) : ::BaseMessage(name, kind)
{
}

GossipMessage::GossipMessage(const GossipMessage& other) : ::BaseMessage(other)
{
    copy(other);
}

GossipMessage::~GossipMessage()
{
}

GossipMessage& GossipMessage::operator=(const GossipMessage& other)
{
    if (this == &other) return *this;
    ::BaseMessage::operator=(other);
    copy(other);
    return *this;
}

void GossipMessage::copy(const GossipMessage& other)
{
    this->content = other.content;
    this->firstTime = other.firstTime;
}

void GossipMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::BaseMessage::parsimPack(b);
    doParsimPacking(b,this->content);
    doParsimPacking(b,this->firstTime);
}

void GossipMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::BaseMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->content);
    doParsimUnpacking(b,this->firstTime);
}

const char * GossipMessage::getContent() const
{
    return this->content.c_str();
}

void GossipMessage::setContent(const char * content)
{
    this->content = content;
}

bool GossipMessage::getFirstTime() const
{
    return this->firstTime;
}

void GossipMessage::setFirstTime(bool firstTime)
{
    this->firstTime = firstTime;
}

class GossipMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_content,
        FIELD_firstTime,
    };
  public:
    GossipMessageDescriptor();
    virtual ~GossipMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(GossipMessageDescriptor)

GossipMessageDescriptor::GossipMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(GossipMessage)), "BaseMessage")
{
    propertyNames = nullptr;
}

GossipMessageDescriptor::~GossipMessageDescriptor()
{
    delete[] propertyNames;
}

bool GossipMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<GossipMessage *>(obj)!=nullptr;
}

const char **GossipMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *GossipMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int GossipMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 2+base->getFieldCount() : 2;
}

unsigned int GossipMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_content
        FD_ISEDITABLE,    // FIELD_firstTime
    };
    return (field >= 0 && field < 2) ? fieldTypeFlags[field] : 0;
}

const char *GossipMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "content",
        "firstTime",
    };
    return (field >= 0 && field < 2) ? fieldNames[field] : nullptr;
}

int GossipMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "content") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "firstTime") == 0) return baseIndex + 1;
    return base ? base->findField(fieldName) : -1;
}

const char *GossipMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_content
        "bool",    // FIELD_firstTime
    };
    return (field >= 0 && field < 2) ? fieldTypeStrings[field] : nullptr;
}

const char **GossipMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *GossipMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int GossipMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    GossipMessage *pp = omnetpp::fromAnyPtr<GossipMessage>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void GossipMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    GossipMessage *pp = omnetpp::fromAnyPtr<GossipMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'GossipMessage'", field);
    }
}

const char *GossipMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    GossipMessage *pp = omnetpp::fromAnyPtr<GossipMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string GossipMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    GossipMessage *pp = omnetpp::fromAnyPtr<GossipMessage>(object); (void)pp;
    switch (field) {
        case FIELD_content: return oppstring2string(pp->getContent());
        case FIELD_firstTime: return bool2string(pp->getFirstTime());
        default: return "";
    }
}

void GossipMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    GossipMessage *pp = omnetpp::fromAnyPtr<GossipMessage>(object); (void)pp;
    switch (field) {
        case FIELD_content: pp->setContent((value)); break;
        case FIELD_firstTime: pp->setFirstTime(string2bool(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'GossipMessage'", field);
    }
}

omnetpp::cValue GossipMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    GossipMessage *pp = omnetpp::fromAnyPtr<GossipMessage>(object); (void)pp;
    switch (field) {
        case FIELD_content: return pp->getContent();
        case FIELD_firstTime: return pp->getFirstTime();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'GossipMessage' as cValue -- field index out of range?", field);
    }
}

void GossipMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    GossipMessage *pp = omnetpp::fromAnyPtr<GossipMessage>(object); (void)pp;
    switch (field) {
        case FIELD_content: pp->setContent(value.stringValue()); break;
        case FIELD_firstTime: pp->setFirstTime(value.boolValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'GossipMessage'", field);
    }
}

const char *GossipMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr GossipMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    GossipMessage *pp = omnetpp::fromAnyPtr<GossipMessage>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void GossipMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    GossipMessage *pp = omnetpp::fromAnyPtr<GossipMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'GossipMessage'", field);
    }
}

Register_Class(RatingMessage)

RatingMessage::RatingMessage(const char *name, short kind) : ::BaseMessage(name, kind)
{
}

RatingMessage::RatingMessage(const RatingMessage& other) : ::BaseMessage(other)
{
    copy(other);
}

RatingMessage::~RatingMessage()
{
}

RatingMessage& RatingMessage::operator=(const RatingMessage& other)
{
    if (this == &other) return *this;
    ::BaseMessage::operator=(other);
    copy(other);
    return *this;
}

void RatingMessage::copy(const RatingMessage& other)
{
    this->serverId = other.serverId;
    this->rating = other.rating;
}

void RatingMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::BaseMessage::parsimPack(b);
    doParsimPacking(b,this->serverId);
    doParsimPacking(b,this->rating);
}

void RatingMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::BaseMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->serverId);
    doParsimUnpacking(b,this->rating);
}

int RatingMessage::getServerId() const
{
    return this->serverId;
}

void RatingMessage::setServerId(int serverId)
{
    this->serverId = serverId;
}

double RatingMessage::getRating() const
{
    return this->rating;
}

void RatingMessage::setRating(double rating)
{
    this->rating = rating;
}

class RatingMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_serverId,
        FIELD_rating,
    };
  public:
    RatingMessageDescriptor();
    virtual ~RatingMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(RatingMessageDescriptor)

RatingMessageDescriptor::RatingMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(RatingMessage)), "BaseMessage")
{
    propertyNames = nullptr;
}

RatingMessageDescriptor::~RatingMessageDescriptor()
{
    delete[] propertyNames;
}

bool RatingMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RatingMessage *>(obj)!=nullptr;
}

const char **RatingMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *RatingMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int RatingMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 2+base->getFieldCount() : 2;
}

unsigned int RatingMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_serverId
        FD_ISEDITABLE,    // FIELD_rating
    };
    return (field >= 0 && field < 2) ? fieldTypeFlags[field] : 0;
}

const char *RatingMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "serverId",
        "rating",
    };
    return (field >= 0 && field < 2) ? fieldNames[field] : nullptr;
}

int RatingMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "serverId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "rating") == 0) return baseIndex + 1;
    return base ? base->findField(fieldName) : -1;
}

const char *RatingMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_serverId
        "double",    // FIELD_rating
    };
    return (field >= 0 && field < 2) ? fieldTypeStrings[field] : nullptr;
}

const char **RatingMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *RatingMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int RatingMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    RatingMessage *pp = omnetpp::fromAnyPtr<RatingMessage>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void RatingMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    RatingMessage *pp = omnetpp::fromAnyPtr<RatingMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'RatingMessage'", field);
    }
}

const char *RatingMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    RatingMessage *pp = omnetpp::fromAnyPtr<RatingMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RatingMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    RatingMessage *pp = omnetpp::fromAnyPtr<RatingMessage>(object); (void)pp;
    switch (field) {
        case FIELD_serverId: return long2string(pp->getServerId());
        case FIELD_rating: return double2string(pp->getRating());
        default: return "";
    }
}

void RatingMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    RatingMessage *pp = omnetpp::fromAnyPtr<RatingMessage>(object); (void)pp;
    switch (field) {
        case FIELD_serverId: pp->setServerId(string2long(value)); break;
        case FIELD_rating: pp->setRating(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RatingMessage'", field);
    }
}

omnetpp::cValue RatingMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    RatingMessage *pp = omnetpp::fromAnyPtr<RatingMessage>(object); (void)pp;
    switch (field) {
        case FIELD_serverId: return pp->getServerId();
        case FIELD_rating: return pp->getRating();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'RatingMessage' as cValue -- field index out of range?", field);
    }
}

void RatingMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    RatingMessage *pp = omnetpp::fromAnyPtr<RatingMessage>(object); (void)pp;
    switch (field) {
        case FIELD_serverId: pp->setServerId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_rating: pp->setRating(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RatingMessage'", field);
    }
}

const char *RatingMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr RatingMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    RatingMessage *pp = omnetpp::fromAnyPtr<RatingMessage>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void RatingMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    RatingMessage *pp = omnetpp::fromAnyPtr<RatingMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RatingMessage'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

