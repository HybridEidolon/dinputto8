/**
* Copyright (C) 2018 Elisha Riedlinger
*
* This software is  provided 'as-is', without any express  or implied  warranty. In no event will the
* authors be held liable for any damages arising from the use of this software.
* Permission  is granted  to anyone  to use  this software  for  any  purpose,  including  commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not claim that you  wrote the
*      original  software. If you use this  software  in a product, an  acknowledgment in the product
*      documentation would be appreciated but is not required.
*   2. Altered source versions must  be plainly  marked as such, and  must not be  misrepresented  as
*      being the original software.
*   3. This notice may not be removed or altered from any source distribution.
*/

#include "dllmain.h"
#include "dinput.h"

#ifndef DINPUTTO8NOLOG
std::ofstream LOG("dinput.log");
#endif

bool InitFlag = false;
DWORD diVersion = 0;

AddressLookupTableDinput<void> ProxyAddressLookupTable = AddressLookupTableDinput<void>();

DirectInput8CreateProc m_pDirectInput8Create = nullptr;
DllCanUnloadNowProc m_pDllCanUnloadNow = nullptr;
DllGetClassObjectProc m_pDllGetClassObject = nullptr;
DllRegisterServerProc m_pDllRegisterServer = nullptr;
DllUnregisterServerProc m_pDllUnregisterServer = nullptr;

void InitDinput8()
{
	// Check if already initialized
	if (InitFlag)
	{
		return;
	}
	InitFlag = true;

	// Load dll
	char path[MAX_PATH];
	strcpy_s(path, "dinput8.dll");
	Log() << "Loading " << path;
	HMODULE dinput8dll = LoadLibraryA(path);

	// Get function addresses
	m_pDirectInput8Create = (DirectInput8CreateProc)GetProcAddress(dinput8dll, "DirectInput8Create");
	m_pDllCanUnloadNow = (DllCanUnloadNowProc)GetProcAddress(dinput8dll, "DllCanUnloadNow");
	m_pDllGetClassObject = (DllGetClassObjectProc)GetProcAddress(dinput8dll, "DllGetClassObject");
	m_pDllRegisterServer = (DllRegisterServerProc)GetProcAddress(dinput8dll, "DllRegisterServer");
	m_pDllUnregisterServer = (DllUnregisterServerProc)GetProcAddress(dinput8dll, "DllUnregisterServer");
}

HRESULT WINAPI DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA* lplpDirectInput, LPUNKNOWN punkOuter)
{
	InitDinput8();

	if (!m_pDirectInput8Create || !lplpDirectInput)
	{
		return E_FAIL;
	}

	Log() << __FUNCTION__ << " Redirecting version " << hex(dwVersion) << " to --> 'DirectInput8Create'";

	HRESULT hr = m_pDirectInput8Create(hinst, 0x0800, IID_IDirectInput8A, (LPVOID*)lplpDirectInput, punkOuter);

	if (SUCCEEDED(hr))
	{
		diVersion = dwVersion;
		*lplpDirectInput = ProxyAddressLookupTable.FindAddress<m_IDirectInputA>(*lplpDirectInput);
	}

	return hr;
}

HRESULT WINAPI DirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riid, LPVOID * lplpDD, LPUNKNOWN punkOuter)
{
	InitDinput8();

	if (!m_pDirectInput8Create || !lplpDD)
	{
		return E_FAIL;
	}

	Log() << __FUNCTION__ << " Redirecting version " << hex(dwVersion) << " to --> 'DirectInput8Create'";

	HRESULT hr = m_pDirectInput8Create(hinst, 0x0800, (GetStringType(riid) == UNICODE) ? IID_IDirectInput8W : IID_IDirectInput8A, lplpDD, punkOuter);

	if (SUCCEEDED(hr))
	{
		diVersion = dwVersion;
		genericQueryInterface(riid, lplpDD);
	}

	return hr;
}

HRESULT WINAPI DirectInputCreateW(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW* lplpDirectInput, LPUNKNOWN punkOuter)
{
	InitDinput8();

	if (!m_pDirectInput8Create || !lplpDirectInput)
	{
		return E_FAIL;
	}

	Log() << __FUNCTION__ << " Redirecting version " << hex(dwVersion) << " to --> 'DirectInput8Create'";

	HRESULT hr = m_pDirectInput8Create(hinst, 0x0800, IID_IDirectInput8W, (LPVOID*)lplpDirectInput, punkOuter);

	if (SUCCEEDED(hr))
	{
		diVersion = dwVersion;
		*lplpDirectInput = ProxyAddressLookupTable.FindAddress<m_IDirectInputW>(*lplpDirectInput);
	}

	return hr;
}

HRESULT WINAPI DllCanUnloadNow()
{
	InitDinput8();

	if (!m_pDllCanUnloadNow)
	{
		return E_FAIL;
	}

	return m_pDllCanUnloadNow();
}

HRESULT WINAPI DllGetClassObject(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv)
{
	InitDinput8();

	if (!m_pDllGetClassObject || !ppv)
	{
		return E_FAIL;
	}

	DWORD StringType = GetStringType(riid);

	HRESULT hr = m_pDllGetClassObject(rclsid, (StringType == ANSI_CHARSET) ? IID_IDirectInput8A : (StringType == UNICODE) ? IID_IDirectInput8W : riid, ppv);

	if (SUCCEEDED(hr))
	{
		genericQueryInterface(riid, ppv);
	}

	return hr;
}

HRESULT WINAPI DllRegisterServer()
{
	InitDinput8();

	if (!m_pDllRegisterServer)
	{
		return E_FAIL;
	}

	return m_pDllRegisterServer();
}

HRESULT WINAPI DllUnregisterServer()
{
	InitDinput8();

	if (!m_pDllUnregisterServer)
	{
		return E_FAIL;
	}

	return m_pDllUnregisterServer();
}
