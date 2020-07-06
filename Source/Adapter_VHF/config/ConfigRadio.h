#ifndef CONFIGRADIO_H
#define CONFIGRADIO_H

#include <stdint.h>

class ConfigRadio
{
public:
    ConfigRadio();
    virtual ~ConfigRadio();
    virtual void load() = 0;

    inline uint8_t  getDataCom() { return m_dataCom; }
    inline uint8_t  getCtrlCom() { return m_ctrlCom; }
    inline uint8_t  getTimerFactor() { return m_timerFactor; }
    inline uint8_t  getCircleDrift() { return m_circleDrift; }
    inline uint16_t getNotInChainCtLmt() { return m_notInChainCtLmt; }

protected:
    virtual void createConfig() = 0;
    virtual void loadConfig() = 0;

protected:
    uint8_t             m_dataCom;
    uint8_t             m_ctrlCom;
    uint8_t             m_timerFactor;
    uint8_t             m_circleDrift;
    uint16_t            m_notInChainCtLmt;
};

#endif // CONFIGRADIO_H
