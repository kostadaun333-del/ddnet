#include <game/client/gameclient.h>
#include <engine/shared/config.h>

void CGameClient::OnMessage(int MsgId, CUnpacker *pUnpacker, int Conn, bool Dummy)
{
    IGameClient::OnMessage(MsgId, pUnpacker, Conn, Dummy);

    if(MsgId == NETMSGTYPE_SV_CHAT)
    {
        CNetMsg_Sv_Chat *pMsg = (CNetMsg_Sv_Chat *)pUnpacker->Unpack();
        if(pMsg->m_ClientId >= 0 && pMsg->m_ClientId < MAX_CLIENTS)
        {
            const char *pText = pMsg->m_pMessage;
            if(str_find_nocase(pText, "софт") || str_find_nocase(pText, "чит") || str_find_nocase(pText, "cheat"))
            {
                char aBuf[256];
                str_format(aBuf, sizeof(aBuf), "NoCheat: Игрок %s написал слово-триггер!", m_aClients[pMsg->m_ClientId].m_aName);
                Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "nocheat", aBuf);
            }
        }
    }
}
