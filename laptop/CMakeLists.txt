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
    Qt::Qt1
    ${X11_LIBRARIES}
)

install(TARGETS kcmlaptop RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kcmlaptop.kdelnk DESTINATION ${KDE1_APPSDIR}/Utilities)
install_icon(kcmlaptop.xpm)
