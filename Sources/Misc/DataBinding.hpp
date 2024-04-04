#pragma once
#include <any>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>

#include "Delegate.hpp"
#include "FileSystem/Logger.hpp"
#include "TypeTraits.hpp"

namespace Sandbox
{
    class IDataBinding
    {
        friend class DataBinding;

    public:
        virtual void Trigger(const DelegateHandle& InDelegateHandle = DelegateHandle::Null) = 0;

        /** 解除对数据原型、绑定函数等相关资源的引用 */
        virtual void Release() = 0;

        virtual bool UnBind(DelegateHandle InDelegateHandle) = 0;

        virtual ~IDataBinding() = default;

    protected:
        IDataBinding(std::string InName);

        // 保存所有存在于Slots中的Handle
        std::unordered_set<DelegateHandle> DelegateHandleSet;
        // 数据绑定的名称
        std::string Name;
    };


    template <typename AbstractData>
    class TDataBinding : public IDataBinding
    {
        friend class DataBinding;

    public:
        ~TDataBinding() override;

        typedef Delegate<AbstractData> DelegateType;  // 用于创建绑定函数的TDelegate签名
        /**
         * 添加数据的绑定函数
         *
         * @param InDelegate 外部构造的绑定函数，推荐使用相关宏构造：CreateLambdaBind、CreateRawBind
         * @return 绑定函数FDelegate的Handle，用于解绑时作为参数传入
         */
        DelegateHandle Bind(DelegateType InDelegate);

        // 使用std::function绑定
        DelegateHandle Bind(const std::function<void(AbstractData)>& func);

        template <typename Instance, void (Instance::*Func)(AbstractData)>
        DelegateHandle BindMember(Instance* obj);

        //~Begin IDataBinding interface
        bool UnBind(DelegateHandle InDelegateHandle) override;

        void Trigger(const DelegateHandle& InDelegateHandle = DelegateHandle::Null) override;

        void Release() override;

        void SetData(AbstractData inData);

        /**
         * 获得数据原型
         *
         * @return 内部的数据原型
         */
        AbstractData GetData();

        // 用于没有参数的成员函数
        TDataBinding(std::string InName, AbstractData InData);

    protected:
        const std::type_info& GetType() const { return typeid(AbstractData); }

        AbstractData data;

        // 保存绑定函数
        std::vector<DelegateType> Slots;
    };


    template <>
    class TDataBinding<void> : public IDataBinding
    {
        friend class DataBinding;

    public:
        ~TDataBinding() override = default;

        typedef Delegate<void> DelegateType;  // 用于创建绑定函数的TDelegate签名
        /**
         * 添加数据的绑定函数
         *
         * @param InDelegate 外部构造的绑定函数，推荐使用相关宏构造：CreateLambdaBind、CreateRawBind
         * @return 绑定函数FDelegate的Handle，用于解绑时作为参数传入
         */
        DelegateHandle Bind(DelegateType InDelegate);

        // 使用std::function绑定
        DelegateHandle Bind(const std::function<void(void)>& func);

        // 用于没有参数的成员函数
        template <typename Instance, void (Instance::*Func)()>
        DelegateHandle BindMember(Instance* obj);


        //~Begin IDataBinding interface
        bool UnBind(DelegateHandle InDelegateHandle) override;

        void Trigger(const DelegateHandle& InDelegateHandle = DelegateHandle::Null) override;

        void Release() override;


        TDataBinding(std::string InName);

    protected:
        // 保存绑定函数
        std::vector<DelegateType> Slots;
    };


    class DataBinding
    {
    public:
        template <typename T>
        static std::shared_ptr<TDataBinding<T>> Get(const std::string& inName);

        template <typename T>
        bool Delete(const std::string& inName);

        template <class AbstractData>
        bool UnBind(std::string InName, DelegateHandle InDelegateHandle);

        template <class AbstractData>
        void Trigger(std::string InName, const DelegateHandle& InDelegateHandle);

        // /**
        //  * 给指定数据绑定解绑函数
        //  * @param InName
        //  * @param InDelegateHandle
        //  * @return
        //  */
        // static bool UnBind(std::string InName, DelegateHandle InDelegateHandle);
        /**
         * 构造指定名称的数据绑定，并传入数据原型，
         * 推荐UObject及其子类模板加裸指针，其他类型结构模板加TSharedPtr，基本数据类型可直接使用
         *
         * @param InName 数据绑定对象自定义名称，同时查找用
         * @param InData 数据原型
         * @return 数据绑定
         */
        template <typename AbstractData, typename std::enable_if<IsSharedPtr<AbstractData>::value, int>::type = 0>
        static auto Create(std::string InName, AbstractData InData) -> std::shared_ptr<TDataBinding<std::shared_ptr<typename ExtractType<AbstractData>::type>>>;

        template <typename AbstractData, typename std::enable_if<!IsSharedPtr<AbstractData>::value, int>::type = 0>
        static auto Create(std::string InName, AbstractData InData) -> std::shared_ptr<TDataBinding<AbstractData>>;

        static auto Create(std::string InName) -> std::shared_ptr<TDataBinding<void>>;

        /**
         * 清除指定名称的数据绑定
         *
         * @param InName 数据绑定对象自定义名称
         * @return 是否成功删除
         */
        // static bool Delete(std::string InName);

        /**
         * 触发指定名称的数据绑定的所有绑定函数
         *
         * @param InName 指定名称的数据绑定
         * @param InDelegateHandle （可选的）若传入绑定函数的 Handle ，以便只调用这个已绑定的函数
         */
        static void Trigger(std::string InName, const DelegateHandle& InDelegateHandle = DelegateHandle());

    protected:
        // 使用std::any来存储任意类型的TDataBinding<T>
        static std::map<std::string, std::any> DataBindingMap;
        // static std::map<std::string, std::shared_ptr<IDataBinding>> DataBindingMap; // 名称到数据绑定的映射
    };


    template <typename AbstractData>
    std::shared_ptr<TDataBinding<AbstractData>> DataBinding::Get(const std::string& inName)
    {
        auto it = DataBindingMap.find(inName);
        if (it == DataBindingMap.end())
        {
            LOGF("No DataBinding found with name: {}", inName)
        }

        try
        {
            // 尝试从std::any中提取std::shared_ptr<TDataBinding<T>>
            auto binding = std::any_cast<std::shared_ptr<TDataBinding<AbstractData>>>(it->second);
            return binding;
        }
        catch (const std::bad_any_cast&)
        {
            LOGF("Failed to cast binding to the requested type for name: {}", inName)
        }
        return nullptr;
    }

    template <typename AbstractData>
    bool DataBinding::Delete(const std::string& inName)
    {
        auto it = DataBinding::DataBindingMap.find(inName);
        if (it != DataBinding::DataBindingMap.end())
        {
            try
            {
                auto DataBindingRef = std::any_cast<std::shared_ptr<TDataBinding<AbstractData>>>(it->second);
                DataBindingRef->Release();
                DataBinding::DataBindingMap.erase(inName);
                return true;
            }
            catch (const std::bad_any_cast&)
            {
                LOGF("Failed to cast binding to the requested type for name: {}", inName)
            }
        }
        return false;
    }

    template <typename AbstractData>
    bool DataBinding::UnBind(std::string InName, DelegateHandle InDelegateHandle)
    {
        auto DataBinding = DataBinding::Get<AbstractData>(InName);
        return DataBinding->UnBind(InDelegateHandle);
    }

    template <typename AbstractData>
    void DataBinding::Trigger(std::string InName, const DelegateHandle& InDelegateHandle)
    {
        if (auto DataBinding = Get<AbstractData>(InName))
        {
            DataBinding->Trigger(InDelegateHandle);
        }
    }


    template <typename AbstractData>
    TDataBinding<AbstractData>::~TDataBinding()
    {
        TDataBinding<AbstractData>::Release();
    }


    template <typename AbstractData>
    DelegateHandle TDataBinding<AbstractData>::Bind(DelegateType InDelegate)
    {
        DelegateHandle DelegateHandle = InDelegate.GetHandle();
        if (!DelegateHandleSet.contains(DelegateHandle))
        {
            Slots.push_back(InDelegate);
            DelegateHandleSet.emplace(DelegateHandle);
        }
        return DelegateHandle;
    }
    template <typename AbstractData>
    DelegateHandle TDataBinding<AbstractData>::Bind(const std::function<void(AbstractData)>& func)
    {
        Delegate<AbstractData> delegate(func);
        return Bind(delegate);
    }

    template <typename AbstractData>
    template <typename Instance, void (Instance::*Func)(AbstractData)>
    DelegateHandle TDataBinding<AbstractData>::BindMember(Instance* obj)
    {
        // 使用 std::bind 创建一个绑定了成员函数和对象的 Delegate
        DelegateType boundDelegate(std::bind(Func, obj, std::placeholders::_1));

        DelegateHandle DelegateHandle = boundDelegate.GetHandle();
        if (!DelegateHandleSet.contains(DelegateHandle))
        {
            Slots.push_back(boundDelegate);
            DelegateHandleSet.emplace(DelegateHandle);
        }
        return DelegateHandle;
    }


    template <typename AbstractData>
    bool TDataBinding<AbstractData>::UnBind(DelegateHandle InDelegateHandle)
    {
        if (DelegateHandleSet.contains(InDelegateHandle))
        {
            DelegateHandleSet.erase(InDelegateHandle);
            for (auto Item = Slots.begin(); Item != Slots.end(); ++Item)
            {
                if (static_cast<DelegateType>(*Item).GetHandle() == InDelegateHandle)
                {
                    Slots.erase(Item);
                    return true;
                }
            }
            std::cerr << "UnBind in DelegateHandleSet, but not in Slots??\n";
        }
        return false;
    }


    template <typename AbstractData>
    void TDataBinding<AbstractData>::Trigger(const DelegateHandle& InDelegateHandle)
    {
        bool bHasInDelegateHandle = InDelegateHandle.IsValid();
        for (DelegateType& Slot : Slots)
        {
            if (!bHasInDelegateHandle || (bHasInDelegateHandle && Slot.GetHandle() == InDelegateHandle))
            {
                Slot(data);
            }
        }
    }

    template <typename AbstractData>
    void TDataBinding<AbstractData>::Release()
    {
        DelegateHandleSet.clear();
        Slots.clear();
    }

    template <typename AbstractData>
    void TDataBinding<AbstractData>::SetData(AbstractData inData)
    {
        this->data = inData;
        Trigger();
    }

    template <typename AbstractData>
    AbstractData TDataBinding<AbstractData>::GetData()
    {
        return data;
    }

    template <typename AbstractData>
    TDataBinding<AbstractData>::TDataBinding(std::string InName, AbstractData InData) : IDataBinding(InName)
    {
        data = InData;
    }


    template <typename Instance, void (Instance::*Func)()>
    DelegateHandle TDataBinding<void>::BindMember(Instance* obj)
    {
        // 使用 std::bind 创建一个绑定了成员函数和对象的 Delegate
        DelegateType boundDelegate(std::bind(Func, obj));

        DelegateHandle DelegateHandle = boundDelegate.GetHandle();
        if (!DelegateHandleSet.contains(DelegateHandle))
        {
            Slots.push_back(boundDelegate);
            DelegateHandleSet.emplace(DelegateHandle);
        }
        return DelegateHandle;
    }

    template <typename AbstractData, typename std::enable_if<IsSharedPtr<AbstractData>::value, int>::type>
    auto DataBinding::Create(std::string InName, AbstractData InData) -> std::shared_ptr<TDataBinding<std::shared_ptr<typename ExtractType<AbstractData>::type>>>
    {
        auto entry = DataBinding::DataBindingMap.find(InName);
        if (entry != DataBinding::DataBindingMap.end())
        {
            try
            {
                return std::any_cast<std::shared_ptr<TDataBinding<std::shared_ptr<typename ExtractType<AbstractData>::type>>>>(entry->second);
            }
            catch (const std::bad_any_cast&)
            {
                Logger::Fatal("Failed to cast binding to the requested type for name: " + InName);
            }
        }
        std::shared_ptr<TDataBinding<std::shared_ptr<typename ExtractType<AbstractData>::type>>> CreatedDataBinding =
            std::make_shared<TDataBinding<std::shared_ptr<typename ExtractType<AbstractData>::type>>>(InName, InData);
        DataBinding::DataBindingMap.emplace(InName, CreatedDataBinding);
        return CreatedDataBinding;
    }

    template <typename AbstractData, typename std::enable_if<!IsSharedPtr<AbstractData>::value, int>::type>
    auto DataBinding::Create(std::string InName, AbstractData InData) -> std::shared_ptr<TDataBinding<AbstractData>>
    {
        auto entry = DataBinding::DataBindingMap.find(InName);
        if (entry != DataBinding::DataBindingMap.end())
        {
            try
            {
                return std::any_cast<std::shared_ptr<TDataBinding<AbstractData>>>(entry->second);
            }
            catch (const std::bad_any_cast&)
            {
                Logger::Fatal("Failed to cast binding to the requested type for name: " + InName);
            }
        }
        std::shared_ptr<TDataBinding<AbstractData>> CreatedDataBinding = std::make_shared<TDataBinding<AbstractData>>(InName, InData);
        DataBinding::DataBindingMap.emplace(InName, CreatedDataBinding);
        return CreatedDataBinding;
    }
}
