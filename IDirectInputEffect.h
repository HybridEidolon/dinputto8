#pragma once

class m_IDirectInputEffect : public IDirectInputEffect, public AddressLookupTableObject
{
private:
	IDirectInputEffect *ProxyInterface;
	m_IDirectInputEffect *WrapperInterface;
	REFIID WrapperID = IID_IDirectInputEffect;

public:
	m_IDirectInputEffect(IDirectInputEffect *aOriginal) : ProxyInterface(aOriginal), WrapperInterface(this)
	{
		ProxyAddressLookupTable.SaveAddress(this, ProxyInterface);

		LogDebug() << "Creating device " << __FUNCTION__;
	}
	~m_IDirectInputEffect()
	{
		ProxyAddressLookupTable.DeleteAddress(this);
	}

	IDirectInputEffect *GetProxyInterface() { return ProxyInterface; }

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInputEffect methods ***/
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD, REFGUID);
	STDMETHOD(GetEffectGuid)(THIS_ LPGUID);
	STDMETHOD(GetParameters)(THIS_ LPDIEFFECT, DWORD);
	STDMETHOD(SetParameters)(THIS_ LPCDIEFFECT, DWORD);
	STDMETHOD(Start)(THIS_ DWORD, DWORD);
	STDMETHOD(Stop)(THIS);
	STDMETHOD(GetEffectStatus)(THIS_ LPDWORD);
	STDMETHOD(Download)(THIS);
	STDMETHOD(Unload)(THIS);
	STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE);
};
