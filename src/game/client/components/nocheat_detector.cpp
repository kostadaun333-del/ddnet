#include <game/client/gameclient.h>

void CGameClient::OnUpdate()
{
    // Вызываем оригинальную функцию (чтобы игра не сломалась)
    IGameClient::OnUpdate();

    // Проходим по всем игрокам
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        // Пропускаем себя и тех, кого нет на сервере
        if(i == m_Snap.m_LocalClientId || !m_Snap.m_apPlayerInfos[i]) 
            continue;

        const CNetObj_Character *pChar = m_Snap.m_aCharacters[i].m_Cur;
        const CNetObj_Character *pPrevChar = m_Snap.m_aCharacters[i].m_Prev;

        if(!pChar || !pPrevChar) continue;

        // Если игрок использует хук
        if(pChar->m_HookState == HOOK_FLYING || pChar->m_HookState == HOOK_GRABBED)
        {
            // Считаем угол хука сейчас и в предыдущем тике
            vec2 HookDir(pChar->m_HookPos.x - pPrevChar->m_HookPos.x, pChar->m_HookPos.y - pPrevChar->m_HookPos.y);
            vec2 PrevHookDir(pPrevChar->m_HookPos.x - pPrevChar->m_HookPos.x, pPrevChar->m_HookPos.y - pPrevChar->m_HookPos.y);
            
            // Вычисляем разницу углов
            float AngleDiff = angle(HookDir) - angle(PrevHookDir);
            
            // Если угол меняется слишком резко (больше 45 градусов за тик)
            if(abs(AngleDiff) > 0.785f) 
            {
                // Выводим в консоль предупреждение
                char aBuf[256];
                str_format(aBuf, sizeof(aBuf), "NoCheat: Игрок %s подозрительно крутит хуком!", m_aClients[i].m_aName);
                Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "nocheat", aBuf);
            }
        }
    }
}
