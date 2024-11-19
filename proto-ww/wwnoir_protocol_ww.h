/* Copyright (c) 2013-2015 Stanislaw Halik <sthalik@misaki.pl>
 * Copyright (c) 2015 Wim Vriend
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */

#pragma once
#include "ui_wwnoir_wwcontrols.h"
#include "api/plugin-api.hpp"

#include <QProcess>
#include <QString>
#include <QMutex>

#include <QDebug>

#include <cinttypes>
//#include "freetrackclient/fttypes.h"

#include "compat/shm.h"
#include "compat/timer.hpp"
#include "options/options.hpp"

#include <memory>

#include <inttypes.h>

#define WIBBLEWOBBLE_HEAP "WWTRACK_SharedMem"
#define WIBBLEWOBBLE_MUTEX "WWTRACK_Mutex"

typedef struct WWData__
{
    float Yaw;   /* positive yaw to the left */
    float Pitch; /* positive pitch up */
    float Roll;  /* positive roll to the left */
    float X;
    float Y;
    float Z;
    float timestamp;
} volatile WWData;


typedef struct WWHeap__
{
  WWData data;
} volatile WWHeap;



using namespace options;

struct settings : opts {
    enum enable_status : int { enable_both, enable_freetrack, enable_npclient, };
    value<int>      used_interface{b, "used-interfaces", (int)enable_both};
    value<bool>     ephemeral_library_location{b, "ephemeral-library-location", false};
    value<bool>     use_custom_location{b, "use-custom-location", false};
    value<QString>  custom_location_pathname{b, "custom-library-location", {}};
    settings() : opts("proto-wibblewobbletrack") {}
};

class wibblewobbletrack : TR, public IProtocol
{
    Q_OBJECT

public:
    wibblewobbletrack() = default;
    ~wibblewobbletrack() override;
    module_status initialize() override;
    void pose(const double* pose, const double*) override;
    QString game_name() override;
private:
    Timer t;
    settings s;
    shm_wrapper shm{ WIBBLEWOBBLE_HEAP, WIBBLEWOBBLE_MUTEX, sizeof(WWHeap) };
    WWHeap* pMemData { (WWHeap*) shm.ptr() };

    QProcess dummyTrackIR;

    int intGameID = -1;
    QString connected_game;
    QMutex game_name_mutex;

    void start_dummy();

public:
    enum class status { starting, stopping };
    [[nodiscard]] module_status set_protocols();
    void clear_protocols();
};

class WWControls: public IProtocolDialog
{
    Q_OBJECT
public:
    WWControls();
    void register_protocol(IProtocol *) {}
    void unregister_protocol() {}
private:
    Ui::UICWWControls ui;
    settings s;
private slots:
    void selectDLL();
    void doOK();
    void doCancel();
    void set_custom_location();
};

class wibblewobbleDll : public Metadata
{
    Q_OBJECT

public:
    QString name() { return tr("wibblewobbletrack"); }
    QIcon icon() { return QIcon(":/images/wibblewobbletrack.png"); }
};
