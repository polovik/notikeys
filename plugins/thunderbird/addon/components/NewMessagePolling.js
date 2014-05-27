Components.utils.import("resource://gre/modules/XPCOMUtils.jsm");
Components.utils.import("resource://gre/modules/ctypes.jsm");

var consoleService = Components.classes['@mozilla.org/consoleservice;1'].getService(Components.interfaces.nsIConsoleService);
var prefBranch = Components.classes["@mozilla.org/preferences-service;1"].getService(Components.interfaces.nsIPrefBranch);

const SRV_RSS = "rss";

function log(msg) {
	if (!prefBranch.getBoolPref("extensions.notikeys.log")) {
		return;
	}
	consoleService.logStringMessage("Notikeys: " + msg);
};

function InformNotikeys() {
	log("onLoad plugin");
};

InformNotikeys.prototype = {
	classDescription: "Notikeys XPCOM component",
	classID: Components.ID("{d50fa17d-c872-4287-8e24-f04392f4e568}"),
	contractID: "@notikeys.com/Notikeys;1",
	QueryInterface: XPCOMUtils.generateQI(),
};

var event = {
	notify: function(timer) {
		log("start checking");
		var messagesCount = 0;
		var acctMgr = Components.classes["@mozilla.org/messenger/account-manager;1"].getService(Components.interfaces.nsIMsgAccountManager);
		var accounts = acctMgr.accounts;
		if (accounts.queryElementAt) {
			// Gecko 17+
			for (var i = 0; i < accounts.length; i++) {
				var account = accounts.queryElementAt(i, Components.interfaces.nsIMsgAccount);
				var rootFolder = account.incomingServer.rootFolder; // nsIMsgFolder
				log("Root folder: " + rootFolder.prettiestName + ". Unread: " + rootFolder.getNumUnread(true));
				log("Root folder: " + rootFolder.prettiestName + ". Flags: 0x" + rootFolder.flags.toString(16) + ". Server type: " + rootFolder.server.type);
				// RSS
				if (rootFolder.server.type == SRV_RSS) {
					if (prefBranch.getBoolPref("extensions.notikeys.showrss")) {
						log("RSS feed checked");
					} else {
						log("RSS feed skipped");
						continue;
					}
				}
				messagesCount = messagesCount + rootFolder.getNumUnread(true);
			}
		} else {
			// Gecko < 17
			log("add checking for Gecko < 17");
			return;
			for (var i = 0; i < accounts.Count(); i++) {
				var account = accounts.QueryElementAt(i, Components.interfaces.nsIMsgAccount);
				// Do something with account
			}
		}

		var lib = ctypes.open("e:\\thunder_lib\\NotificationLib.dll");
		log("NotificationLib is opened");
		var notify = lib.declare("notify", ctypes.default_abi, ctypes.void_t, ctypes.int32_t, ctypes.int32_t);
		notify(3, messagesCount);
		log("Notikeys is notified. Events = " + messagesCount);
		lib.close();
	}
}

if (XPCOMUtils.generateNSGetFactory)
	const NSGetFactory = XPCOMUtils.generateNSGetFactory([InformNotikeys]);
else
	const NSGetModule = XPCOMUtils.generateNSGetModule([InformNotikeys]); //for Gecko 1.9.*

var pollTimeout = prefBranch.getIntPref("extensions.notikeys.pollTimeout");
log("pollTimeout = " + pollTimeout);
var timer = Components.classes["@mozilla.org/timer;1"].createInstance(Components.interfaces.nsITimer);
timer.initWithCallback(event, pollTimeout, Components.interfaces.nsITimer.TYPE_REPEATING_PRECISE_CAN_SKIP);