#add_subdirectory(pics)

include_directories (
    ${KDE1_INCLUDE_DIR}
)

set(kcmlaptop_SRCS
    main.cpp power.cpp pcmcia.cpp warning.cpp battery.cpp kbattery.cpp
    daemon.cpp notify.cpp pmlib.c
    )

set(kcmlaptop_MOC
    power.moc pcmcia.moc warning.moc battery.moc kbattery.moc daemon.moc notify.moc
)

qt1_wrap_moc(MOC_FILES ${kcmlaptop_MOC})

add_executable(kcmlaptop ${kcmlaptop_SRCS} ${MOC_FILES})
target_link_libraries(kcmlaptop
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFILE}
    ${KDE1_MEDIATOOL}
    Qt::Qt1
    PkgConfig::libsystemd
    X11::Xss
)

install(TARGETS kcmlaptop RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES
    applnk/Settings/Laptop/.directory
    applnk/Settings/Laptop/pcmcia.kdelnk
    applnk/Settings/Laptop/power.kdelnk
    applnk/Settings/Laptop/battery.kdelnk
    applnk/Settings/Laptop/bwarning.kdelnk
    applnk/Settings/Laptop/cwarning.kdelnk
    DESTINATION ${KDE1_APPSDIR}/Settings/Laptop/)
install_icon(kcmlaptop.xpm)

install_icon(power.xpm)
install_icon(mini-power.xpm)
install_icon(pcmcia.xpm)
install_icon(mini-pcmcia.xpm)
install_icon(laptop_settings.xpm)
install_icon(mini-laptop_settings.xpm)
install_icon(laptop_battery.xpm)
install_icon(mini-laptop_battery.xpm)
install_icon(laptop_charge.xpm)
install_icon(mini-laptop_charge.xpm)
install_icon(laptop_nobattery.xpm)
install_icon(mini-laptop_nobattery.xpm)
install_icon(laptop_nocharge.xpm)
install_icon(mini-laptop_nocharge.xpm)

install(FILES
    doc/en/laptop-1.html
    doc/en/laptop-2.html
    doc/en/laptop-3.html
    doc/en/laptop-4.html
    doc/en/laptop-5.html
    doc/en/laptop-6.html
    doc/en/laptop-7.html
    doc/en/laptop-8.html
    doc/en/laptop.html
    DESTINATION ${KDE1_HTMLDIR}/en/kcontrol/kcmlaptop )
