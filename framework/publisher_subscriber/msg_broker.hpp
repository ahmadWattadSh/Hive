/*******************************************************************************
* Author: Amit
* Date: 01.04.24
* Version: 0 
* Description: Publisher-SUBSCRIBERr, Dispatcher-Callback API
*******************************************************************************/
#ifndef __ILRD_HRD32_MSG_BROKER_HPP__
#define __ILRD_HRD32_MSG_BROKER_HPP__

#include <cstddef> //size_t 
#include <vector> //vector 
#include <algorithm> //vector 


namespace ilrd
{

template<typename MSG>    
class ICallBack;

template <typename MSG> 
class Dispatcher
{
public:
    explicit Dispatcher() = default;
    ~Dispatcher() = default;

    Dispatcher(const Dispatcher&) = delete;
    Dispatcher& operator=(const Dispatcher&) = delete;
    Dispatcher(Dispatcher&&) = delete;
    Dispatcher& operator=(Dispatcher&&) = delete;

    void NotifyAll(const MSG& msg_);

private:

    void Subscribe(ICallBack<MSG>* cb_);
    void UnSubscribe(ICallBack<MSG>* cb_);
    void NotifyDiscAll();
    friend class ICallBack<MSG>;
    
    std::vector<ICallBack<MSG>*> m_SUBSCRIBERrs;
};


template <typename MSG>
class ICallBack
{
public:
    explicit ICallBack(Dispatcher<MSG>* const disp_);
    virtual ~ICallBack();

    ICallBack(const ICallBack& o_) = delete;
    ICallBack& operator=(const ICallBack& o_) = delete;


private:
    friend class Dispatcher<MSG>;
    virtual void Notify(const MSG& msg_) = 0;
    virtual void NotifyDisc() = 0; 
    
     Dispatcher<MSG>* const m_dispatcher;
};


// SUBSRIBER must have Update method (for Notify)
// SUBSRIBER Disconnect method is optional (for NotifyDisc)
template <typename MSG, typename SUBSCRIBER>
class CallBack : public ICallBack<MSG>
{
public:
    using ActionMethod = void(SUBSCRIBER::*)(const MSG&);
    using DisconnectMethod = void(SUBSCRIBER::*)();

    explicit CallBack(Dispatcher<MSG>* const disp_,
                        SUBSCRIBER& sub_,
                        ActionMethod ActionMethod_,
                        DisconnectMethod DisMethod_= nullptr);

    ~CallBack() override;

private:
    void Notify(const MSG& msg_) override;
    void NotifyDisc() override; 

    ActionMethod m_ActionMethod;
    DisconnectMethod m_DisMethod;
    
    SUBSCRIBER& m_sub;
};

//-----------------------------------------------------------------------------------------------------

template <typename MSG> 
void Dispatcher<MSG>::NotifyAll(const MSG& msg_)
{
    std::for_each(m_SUBSCRIBERrs.begin(), m_SUBSCRIBERrs.end(), [&](ICallBack<MSG>* cb) {
        cb->Notify(msg_);
    });
}

template <typename MSG> 
void Dispatcher<MSG>::Subscribe(ICallBack<MSG>* cb_)
{
    m_SUBSCRIBERrs.push_back(cb_);

}

template <typename MSG> 
void Dispatcher<MSG>::UnSubscribe(ICallBack<MSG>* cb_)
{
    auto it = std::find(m_SUBSCRIBERrs.begin(), m_SUBSCRIBERrs.end(), cb_);

    //to do exceptions
    m_SUBSCRIBERrs.erase(it);
}


template <typename MSG> 
void ilrd::Dispatcher<MSG>::NotifyDiscAll()
{
    std::for_each(m_SUBSCRIBERrs.begin(), m_SUBSCRIBERrs.end(), [](ICallBack<MSG>* cb) {
        cb->NotifyDisc();
    });
}

//-------------------------------------------------------------------

template <typename MSG>
ICallBack<MSG>::ICallBack(Dispatcher<MSG>* const disp_):m_dispatcher(disp_)
{
    m_dispatcher->Subscribe(this);
}

template <typename MSG>
ICallBack<MSG>::~ICallBack()
{
    m_dispatcher->UnSubscribe(this);
}


//-------------------------------------------------------------------

template <typename MSG, typename SUBSCRIBER>
CallBack<MSG, SUBSCRIBER>::CallBack( Dispatcher<MSG>*const disp_,
                        SUBSCRIBER& sub_,
                        ActionMethod ActionMethod_,
                        DisconnectMethod DisMethod_)
    : ICallBack<MSG>(disp_), m_ActionMethod(ActionMethod_), m_DisMethod(DisMethod_), m_sub(sub_)
{
}

template <typename MSG, typename SUBSCRIBER>
CallBack<MSG, SUBSCRIBER>::~CallBack()
{
    NotifyDisc();
}

template <typename MSG, typename SUBSCRIBER>
void CallBack<MSG, SUBSCRIBER>::Notify(const MSG& msg_)
{
    (m_sub.*m_ActionMethod)(msg_);
}

template <typename MSG, typename SUBSCRIBER>
void CallBack<MSG, SUBSCRIBER>::NotifyDisc()
{

    (m_sub.*m_DisMethod)();
}



}

#endif// __ILRD_HRD32_MSG_BROKER_HPP__

