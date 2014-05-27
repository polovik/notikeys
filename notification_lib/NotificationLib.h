#ifndef NOTIFICATIONLIB_H
#define NOTIFICATIONLIB_H

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) void notify(int pluginUID, int eventsCount);

#ifdef __cplusplus
}
#endif

#endif // NOTIFICATIONLIB_H
