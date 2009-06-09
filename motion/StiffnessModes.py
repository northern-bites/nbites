
STANDUP_GAINS_VALUE = 1.0
GAINS_ON_VALUE = 0.85
GAINS_ARMS_VALUE = 0.2
GAINS_OFF_VALUE = -1.0
LOWER_HEAD_VALUE = 0.8

LOW_HEAD_STIFFNESSES = (LOWER_HEAD_VALUE, # Head
                        LOWER_HEAD_VALUE,
                        GAINS_ARMS_VALUE, # larm
                        GAINS_ARMS_VALUE,
                        GAINS_ARMS_VALUE,
                        GAINS_ARMS_VALUE,
                        GAINS_ON_VALUE, # lleg
                        GAINS_ON_VALUE,
                        GAINS_ON_VALUE,
                        GAINS_ON_VALUE,
                        GAINS_ON_VALUE,
                        GAINS_ON_VALUE,
                        GAINS_ON_VALUE, # rleg
                        GAINS_ON_VALUE,
                        GAINS_ON_VALUE,
                        GAINS_ON_VALUE,
                        GAINS_ON_VALUE,
                        GAINS_ON_VALUE,
                        GAINS_ARMS_VALUE, # rarm
                        GAINS_ARMS_VALUE,
                        GAINS_ARMS_VALUE,
                        GAINS_ARMS_VALUE,)




STANDUP_STIFFNESSES = (STANDUP_GAINS_VALUE, # Head
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE, # larm
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE, # lleg
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE, # rleg
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE, # rarm
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE,
                       STANDUP_GAINS_VALUE,)

LOOSE_ARMS_STIFFNESSES = (GAINS_ON_VALUE, # Head
                          GAINS_ON_VALUE,
                          GAINS_ARMS_VALUE, # larm
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ON_VALUE, # lleg
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE, # rleg
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ARMS_VALUE, # rarm
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,)

NORMAL_STIFFNESSES = (GAINS_ON_VALUE, # Head
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE, # larm
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE, # lleg
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE, # rleg
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE, # rarm
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE,
                      GAINS_ON_VALUE,)

NO_STIFFNESSES = (GAINS_OFF_VALUE, # Head
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE, # larm
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE, # lleg
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE, # rleg
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE, # rarm
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE,
                      GAINS_OFF_VALUE,)

FAR_KICK_SUPPORT_LEG_VALUE = 0.6
LEFT_FAR_KICK_STIFFNESSES = (GAINS_ON_VALUE, # Head
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE, # larm
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE, # lleg
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,
                             FAR_KICK_SUPPORT_LEG_VALUE, # rleg
                             FAR_KICK_SUPPORT_LEG_VALUE,
                             FAR_KICK_SUPPORT_LEG_VALUE,
                             0.4,
                             FAR_KICK_SUPPORT_LEG_VALUE,
                             0.4,
                             GAINS_ON_VALUE, # rarm
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,)

LOW_LEG_STIFFNESSES =  (GAINS_OFF_VALUE, # Head
                          GAINS_OFF_VALUE,
                          GAINS_ARMS_VALUE, # larm
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE, # lleg
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE, # rleg
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE, # rarm
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,)


FAR_KICK_SUPPORT_LEG_VALUE = 0.6
RIGHT_FAR_KICK_STIFFNESSES = (GAINS_ON_VALUE, # Head
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE, # larm
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,
                             FAR_KICK_SUPPORT_LEG_VALUE, # lleg
                             FAR_KICK_SUPPORT_LEG_VALUE,
                             FAR_KICK_SUPPORT_LEG_VALUE,
                              0.4,
                              FAR_KICK_SUPPORT_LEG_VALUE,
                              0.4,
                             GAINS_ON_VALUE, # rleg
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE, # rarm
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,
                             GAINS_ON_VALUE,)

NO_HEAD_STIFFNESSES = (GAINS_OFF_VALUE, # Head
                          GAINS_OFF_VALUE,
                          GAINS_ARMS_VALUE, # larm
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ON_VALUE, # lleg
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE, # rleg
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ARMS_VALUE, # rarm
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,)

LEFT_SIDE_KICK_STIFFNESSES = LOOSE_ARMS_STIFFNESSES

RIGHT_SIDE_KICK_STIFFNESSES = LOOSE_ARMS_STIFFNESSES

MAKE_KICK_STIFFNESSES = (GAINS_OFF_VALUE, # Head
                          GAINS_OFF_VALUE,
                          GAINS_ARMS_VALUE, # larm
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ON_VALUE, # lleg
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE, # rleg
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ON_VALUE,
                          GAINS_ARMS_VALUE, # rarm
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,
                          GAINS_ARMS_VALUE,)

