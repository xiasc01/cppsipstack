/* 
 * Copyright (C) 2012 Yee Young Han <websearch@naver.com> (http://blog.naver.com/websearch)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

#include "StdAfx.h"
#include "SipUserAgentMFC.h"
#include "SipUserAgentMFCDefine.h"
#include "SipUserAgentMFCEventClass.h"

CSipUserAgentMFC::CSipUserAgentMFC() : m_hWnd(0), m_pclsCallBack(NULL)
{
}

CSipUserAgentMFC::~CSipUserAgentMFC()
{
}

void CSipUserAgentMFC::SetWindowHandle( HWND hWnd )
{
	m_hWnd = hWnd;
}

void CSipUserAgentMFC::SetCallBack( ISipUserAgentCallBack * pclsCallBack )
{
	m_pclsCallBack = pclsCallBack;
}

LRESULT CSipUserAgentMFC::OnSipMessage( WPARAM wParam, LPARAM lParam )
{
	if( m_pclsCallBack )
	{
		switch( wParam )
		{
		case SMC_REGISER:
			{
				CEventRegister * pclsParam = (CEventRegister *)lParam;
				m_pclsCallBack->EventRegister( &pclsParam->m_clsInfo, pclsParam->m_iStatus );
				delete pclsParam;
			}
			break;
		case SMC_INCOMING_CALL:
			{
				CEventIncomingCall * pclsParam = (CEventIncomingCall *)lParam;
				m_pclsCallBack->EventIncomingCall( pclsParam->m_pszCallId, pclsParam->m_pszFrom, pclsParam->m_pszTo, pclsParam->m_pclsRtp );
			}
			break;
		case SMC_CALL_RINGING:
			{
				CEventCallRing * pclsParam = (CEventCallRing *)lParam;
				m_pclsCallBack->EventCallRing( pclsParam->m_pszCallId, pclsParam->m_iSipStatus, pclsParam->m_pclsRtp );
			}
			break;
		case SMC_CALL_START:
			{
				CEventCallStart * pclsParam = (CEventCallStart *)lParam;
				m_pclsCallBack->EventCallStart( pclsParam->m_pszCallId, pclsParam->m_pclsRtp );
			}
			break;
		case SMC_CALL_END:
			{
				CEventCallEnd * pclsParam = (CEventCallEnd *)lParam;
				m_pclsCallBack->EventCallEnd( pclsParam->m_pszCallId, pclsParam->m_iSipStatus );
			}
			break;
		}
	}

	return WMR_TRUE;
}

/**
 * @ingroup SipClientMFC
 * @brief SIP REGISTER 응답 메시지 수신 이벤트 핸들러
 * @param pclsInfo	SIP REGISTER 응답 메시지를 전송한 IP-PBX 정보 저장 객체
 * @param iStatus		SIP REGISTER 응답 코드
 */
void CSipUserAgentMFC::EventRegister( CSipServerInfo * pclsInfo, int iStatus )
{
	CEventRegister * pclsParam = new CEventRegister( pclsInfo, iStatus );
	if( pclsParam == NULL ) return;

	_SendMessage( SMC_REGISER, (LPARAM)pclsParam );
}

/**
 * @ingroup SipClientMFC
 * @brief SIP 통화 요청 수신에 대한 인증 확인 이벤트 핸들러
 * @param pclsMessage	SIP INVITE 요청 메시지
 * @return 인증에 성공하면 true 를 리턴하고 그렇지 않으면 false 를 리턴한다.
 */
bool CSipUserAgentMFC::EventIncomingRequestAuth( CSipMessage * pclsMessage )
{
	return true;
}

/**
 * @ingroup SipClientMFC
 * @brief SIP 통화 요청 수신 이벤트 핸들러
 * @param	pszCallId	SIP Call-ID
 * @param pszFrom		SIP From 사용자 아이디
 * @param pszTo			SIP To 사용자 아이디
 * @param pclsRtp		RTP 정보 저장 객체
 */
void CSipUserAgentMFC::EventIncomingCall( const char * pszCallId, const char * pszFrom, const char * pszTo, CSipCallRtp * pclsRtp )
{
	CEventIncomingCall clsParam( pszCallId, pszFrom, pszTo, pclsRtp );

	_SendMessage( SMC_INCOMING_CALL, (LPARAM)&clsParam );
}

/**
 * @ingroup SipClientMFC
 * @brief SIP Ring / Session Progress 수신 이벤트 핸들러
 * @param	pszCallId		SIP Call-ID
 * @param iSipStatus	SIP 응답 코드
 * @param pclsRtp			RTP 정보 저장 객체
 */
void CSipUserAgentMFC::EventCallRing( const char * pszCallId, int iSipStatus, CSipCallRtp * pclsRtp )
{
	CEventCallRing clsParam( pszCallId, iSipStatus, pclsRtp );

	_SendMessage( SMC_CALL_RINGING, (LPARAM)&clsParam );
}

/**
 * @ingroup SipClientMFC
 * @brief SIP 통화 연결 이벤트 핸들러
 * @param	pszCallId	SIP Call-ID
 * @param pclsRtp		RTP 정보 저장 객체
 */
void CSipUserAgentMFC::EventCallStart( const char * pszCallId, CSipCallRtp * pclsRtp )
{
	CEventCallStart clsParam( pszCallId, pclsRtp );

	_SendMessage( SMC_CALL_START, (LPARAM)&clsParam );
}

/**
 * @ingroup SipClientMFC
 * @brief SIP 통화 종료 이벤트 핸들러
 * @param	pszCallId		SIP Call-ID
 * @param iSipStatus	SIP 응답 코드. INVITE 에 대한 오류 응답으로 전화가 종료된 경우, INVITE 의 응답 코드를 저장한다.
 */
void CSipUserAgentMFC::EventCallEnd( const char * pszCallId, int iSipStatus )
{
	CEventCallEnd clsParam( pszCallId, iSipStatus );

	_SendMessage( SMC_CALL_END, (LPARAM)&clsParam );
}

/**
 * @ingroup SipClientMFC
 * @brief SIP ReINVITE 수신 이벤트 핸들러
 * @param	pszCallId	SIP Call-ID
 * @param pclsRtp		RTP 정보 저장 객체
 */
void CSipUserAgentMFC::EventReInvite( const char * pszCallId, CSipCallRtp * pclsRtp )
{
}

/**
 * @ingroup SipClientMFC
 * @brief Screened / Unscreened Transfer 요청 수신 이벤트 핸들러
 * @param pszCallId					SIP Call-ID
 * @param pszReferToCallId	전화가 전달될 SIP Call-ID
 * @param bScreenedTransfer Screened Transfer 이면 true 가 입력되고 Unscreened Transfer 이면 false 가 입력된다.
 * @returns 요청을 수락하면 true 를 리턴하고 그렇지 않으면 false 를 리턴한다.
 */
bool CSipUserAgentMFC::EventTransfer( const char * pszCallId, const char * pszReferToCallId, bool bScreenedTransfer )
{
	return false;
}

/**
 * @ingroup SipClientMFC
 * @brief Blind Transfer 요청 수신 이벤트 핸들러
 * @param pszCallId			SIP Call-ID
 * @param pszReferToId	전화가 전달될 사용자 아이디
 * @returns 요청을 수락하면 true 를 리턴하고 그렇지 않으면 false 를 리턴한다.
 */
bool CSipUserAgentMFC::EventBlindTransfer( const char * pszCallId, const char * pszReferToId )
{
	return false;
}

/**
 * @ingroup SipClientMFC
 * @brief SIP MESSAGE 수신 이벤트 핸들러
 * @param pszFrom			SIP 메시지 전송 아이디
 * @param pszTo				SIP 메시지 수신 아이디
 * @param pclsMessage SIP 메시지
 * @returns 요청을 수락하면 true 를 리턴하고 그렇지 않으면 false 를 리턴한다.
 */
bool CSipUserAgentMFC::EventMessage( const char * pszFrom, const char * pszTo, CSipMessage * pclsMessage )
{
	return false;
}

/**
 * @ingroup SipClientMFC
 * @brief SIP 메시지 수신 쓰레드가 종료됨을 알려주는 이벤트 핸들러
 * @param iThreadId UDP 쓰레드 번호
 */
void CSipUserAgentMFC::EventCallBackThreadEnd( int iThreadId )
{
}

/**
 * @ingroup SipClientMFC
 * @brief 윈도우 메시지로 SIP 이벤트를 전달한다.
 * @param wParam SIP 이벤트 명령 코드
 * @param lParam SIP 이벤트 데이터
 * @returns 성공하면 WMR_TRUE 를 리턴하고 실패하면 WMR_FALSE 를 리턴한다.
 */
LRESULT CSipUserAgentMFC::_SendMessage( WPARAM wParam, LPARAM lParam )
{
	if( m_hWnd == 0 ) return WMR_FALSE;

	if( wParam == SMC_REGISER )
	{
		PostMessage( m_hWnd, SIP_MESSAGE_ID, wParam, lParam );
		return WMR_TRUE;
	}

	return SendMessage( m_hWnd, SIP_MESSAGE_ID, wParam, lParam );
}
