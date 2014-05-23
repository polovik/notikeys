/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 */


const MAILATTENTION_ADDON_ID = "mail-attention@giovanni.coriasco";

const MAILATTENTION_SRV_RSS = "rss";

const MAILATTENTION_MENUITEM_NEWS = "notikeys-menuitem-news";
const MAILATTENTION_MENUITEM_RSS = "notikeys-menuitem-rss";
const MAILATTENTION_MENUITEM_LOG = "notikeys-menuitem-log";
const MAILATTENTION_MENUITEM_RESTORE = "notikeys-menuitem-restore";
const MAILATTENTION_MENUITEM_ENABLE = "notikeys-menuitem-enable";

Components.utils.import("resource://gre/modules/ctypes.jsm")

var notikeys = {
	checkNewMails: function() {
		this.log("checkNewMails")
		var acctMgr = Components.classes["@mozilla.org/messenger/account-manager;1"].getService(Components.interfaces.nsIMsgAccountManager);
		var accounts = acctMgr.accounts;
		if (accounts.queryElementAt) {
			// Gecko 17+
//			this.log("Gecko 17+");
			for (var i = 0; i < accounts.length; i++) {
				var account = accounts.queryElementAt(i, Components.interfaces.nsIMsgAccount);
				var rootFolder = account.incomingServer.rootFolder; // nsIMsgFolder
				this.log("Root folder: " + rootFolder.prettiestName + ". Unread: " + rootFolder.getNumUnread(true));
				this.log("Root folder: " + rootFolder.prettiestName + ". Flags: 0x" + rootFolder.flags.toString(16) + ". Server type: " + rootFolder.server.type);
				// RSS
				if (rootFolder.server.type == MAILATTENTION_SRV_RSS){
					if (this.prefBranch.getBoolPref(this.menuitemPrefs[MAILATTENTION_MENUITEM_RSS])){
						this.log("RSS feed checked");
					} else {
						this.log("RSS feed skipped");
					}
				}
/*				if (rootFolder.hasSubFolders) {
					var subFolders = rootFolder.subFolders; // nsIMsgFolder
					while (subFolders.hasMoreElements()) {
						var folder = subFolders.getNext().QueryInterface(Components.interfaces.nsIMsgFolder);
						this.log("Subfolder: " + folder.prettiestName + ".Unread: " + rootFolder.getNumUnread(true));
					}
				}
*/				// Do something with account
			}
		} else {
			// Gecko < 17
			for (var i = 0; i < accounts.Count(); i++) {
				var account = accounts.QueryElementAt(i, Components.interfaces.nsIMsgAccount);
				// Do something with account
			}
		}
		var lib = ctypes.open("C:\\WINDOWS\\system32\\user32.dll");
		
		/* Declare the signature of the function we are going to call */
		var msgBox = lib.declare("MessageBoxW",
		                         ctypes.winapi_abi,
		                         ctypes.int32_t,
		                         ctypes.int32_t,
		                         ctypes.jschar.ptr,
		                         ctypes.jschar.ptr,
		                         ctypes.int32_t);
		var MB_OK = 0;
		
//		var ret = msgBox(0, "Hello world", "title_me", MB_OK);
		
		lib.close();
	},
	
	deinit: function() {
		this.log("notikeys::deinit()");
		window.removeEventListener("load", notikeys.windowListeners.load, false);
		window.removeEventListener("unload", notikeys.windowListeners.unload, false);
		clearInterval(this.timerCheck)
	},
	
	init: function() {
		// initialization code
		// Components initialization
		this.prefBranch = Components.classes["@mozilla.org/preferences-service;1"].getService(Components.interfaces.nsIPrefBranch);
		this.consoleService = Components.classes['@mozilla.org/consoleservice;1'].getService(Components.interfaces.nsIConsoleService);

		////////////////////////////////
		
		this.log("notikeys::init");
		this.timerCheck = setInterval(function() { notikeys.checkNewMails() }, 5000);
		this.log("notikeys::init Timer");
	},
	
	log: function(msg) {
		if(!this.prefBranch.getBoolPref(this.menuitemPrefs[MAILATTENTION_MENUITEM_LOG])) {
			return;
		}
		this.consoleService.logStringMessage(msg);
	},

	 
	QueryInterface: function(aIID) {
		this.log("notikeys::QueryInterface");
		// notikeys implements 'nsISupports' and 'nsIMsgFolderListener' interfaces...
		if (aIID.equals(Components.interfaces.nsISupports) || aIID.equals(Components.interfaces.nsIMsgFolderListener)) {
			return this;
		}

		throw Components.results.NS_ERROR_NO_INTERFACE;
	}
};


// Definitions
notikeys.menuitemPrefs = new Array();
notikeys.menuitemPrefs[MAILATTENTION_MENUITEM_NEWS] = "extensions.notikeys.shownews";
notikeys.menuitemPrefs[MAILATTENTION_MENUITEM_RSS] = "extensions.notikeys.showrss";
notikeys.menuitemPrefs[MAILATTENTION_MENUITEM_LOG] = "extensions.notikeys.log";
notikeys.menuitemPrefs[MAILATTENTION_MENUITEM_RESTORE] = "extensions.notikeys.restore";
notikeys.menuitemPrefs[MAILATTENTION_MENUITEM_ENABLE] = "extensions.notikeys.enable";

notikeys.windowListeners = {
	load:   function() { notikeys.init(); },
	unload: function() { notikeys.deinit(); }
};

window.addEventListener("load", notikeys.windowListeners.load, false);
window.addEventListener("unload", notikeys.windowListeners.unload, false);

