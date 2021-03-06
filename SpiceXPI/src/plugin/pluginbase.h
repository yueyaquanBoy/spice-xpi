/* ***** BEGIN LICENSE BLOCK *****
 *   Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 *   The contents of this file are subject to the Mozilla Public License Version
 *   1.1 (the "License"); you may not use this file except in compliance with
 *   the License. You may obtain a copy of the License at
 *   http://www.mozilla.org/MPL/
 *
 *   Software distributed under the License is distributed on an "AS IS" basis,
 *   WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 *   for the specific language governing rights and limitations under the
 *   License.
 *
 *   Copyright 2009-2011, Red Hat Inc.
 *   Based on mozilla.org's scriptable plugin example
 *
 *   The Original Code is mozilla.org code.
 *
 *   The Initial Developer of the Original Code is
 *   Netscape Communications Corporation.
 *   Portions created by the Initial Developer are Copyright (C) 1998
 *   the Initial Developer. All Rights Reserved.
 *
 *   Contributor(s):
 *   Uri Lublin
 *   Martin Stransky
 *   Peter Hatina
 *
 *   Alternatively, the contents of this file may be used under the terms of
 *   either the GNU General Public License Version 2 or later (the "GPL"), or
 *   the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 *   in which case the provisions of the GPL or the LGPL are applicable instead
 *   of those above. If you wish to allow use of your version of this file only
 *   under the terms of either the GPL or the LGPL, and not to allow others to
 *   use your version of this file under the terms of the MPL, indicate your
 *   decision by deleting the provisions above and replace them with the notice
 *   and other provisions required by the GPL or the LGPL. If you do not delete
 *   the provisions above, a recipient may use your version of this file under
 *   the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef PLUGIN_INSTANCE_BASE_H
#define PLUGIN_INSTANCE_BASE_H

#include <npapi.h>
#include <npruntime.h>

struct nsPluginCreateData
{
    NPP instance;
    NPMIMEType type;
    uint16_t mode;
    int16_t argc;
    char **argn;
    char **argv;
    NPSavedData *saved;
};

class nsPluginInstanceBase
{
public:
    nsPluginInstanceBase();
    virtual ~nsPluginInstanceBase();

    // these three methods must be implemented in the derived
    // class platform specific way
    virtual NPBool init(NPWindow *aWindow) = 0;
    virtual void shut() = 0;
    virtual NPBool isInitialized() = 0;

    // implement all or part of those methods in the derived
    // class as needed
    virtual NPError SetWindow(NPWindow *pNPWindow);
    virtual NPError NewStream(NPMIMEType type, NPStream *stream,
                              NPBool seekable, uint16_t *stype);
    virtual NPError DestroyStream(NPStream *stream, NPError reason);
    virtual void StreamAsFile(NPStream *stream, const char *fname);
    virtual int32_t WriteReady(NPStream *stream);
    virtual int32_t Write(NPStream *stream, int32_t offset,
                          int32_t len, void *buffer);
    virtual void Print(NPPrint *printInfo);
    virtual uint16_t HandleEvent(void *event);
    virtual void URLNotify(const char *url, NPReason reason,
                           void *notifyData);
    virtual NPError GetValue(NPPVariable variable, void *value);
    virtual NPError SetValue(NPNVariable variable, void *value);
};

// functions that should be implemented for each specific plugin

// creation and destruction of the object of the derived class
nsPluginInstanceBase *NS_NewPluginInstance(nsPluginCreateData *aCreateDataStruct);
void NS_DestroyPluginInstance(nsPluginInstanceBase *aPlugin);

// global plugin initialization and shutdown
NPError NS_PluginInitialize();
void NS_PluginShutdown();

#ifdef XP_UNIX
// global to get plugins name & description 
NPError NS_PluginGetValue(NPPVariable aVariable, void *aValue);
#endif

#endif // PLUGIN_INSTANCE_BASE_H
