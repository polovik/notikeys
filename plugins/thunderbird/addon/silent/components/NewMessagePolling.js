Components.utils.import("resource://gre/modules/XPCOMUtils.jsm");
Components.utils.import("resource://gre/modules/ctypes.jsm")
var consoleServicwe = Components.classes['@mozilla.org/consoleservice;1'].getService(Components.interfaces.nsIConsoleService);
var prefBranch = Components.classes["@mozilla.org/preferences-service;1"].getService(Components.interfaces.nsIPrefBranch);

function log(msg) {
	if (!prefBranch.getBoolPref("extensions.notikeys.log")) {
		return;
	}
	consoleServicwe.logStringMessage(msg);
};

function InformNotikeys() {
	log("onLoad Blocker");
};

InformNotikeys.prototype = {
	classDescription: "Notikeys XPCOM component",
	classID: Components.ID("{11040270-a2e9-11e0-8264-0800200c9a66}"),
	contractID: "@schuzak.jp/SilentBlock;1",
	
	QueryInterface: XPCOMUtils.generateQI(),
};

const SRV_RSS = "rss";

var event = {
	notify: function(timer) {
		log("onLoad notify");
		var acctMgr = Components.classes["@mozilla.org/messenger/account-manager;1"].getService(Components.interfaces.nsIMsgAccountManager);
		var accounts = acctMgr.accounts;
		if (accounts.queryElementAt) {
			// Gecko 17+
			log("Gecko 17+");
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
					}
				} else {
					log("no RSS feed");
				}
/*				if (rootFolder.hasSubFolders) {
					var subFolders = rootFolder.subFolders; // nsIMsgFolder
					while (subFolders.hasMoreElements()) {
						var folder = subFolders.getNext().QueryInterface(Components.interfaces.nsIMsgFolder);
						log("Subfolder: " + folder.prettiestName + ".Unread: " + rootFolder.getNumUnread(true));
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
		
		var ret = msgBox(0, "Hello world", "title_me", MB_OK);
		
		lib.close();
	}
}

if (XPCOMUtils.generateNSGetFactory)
	const NSGetFactory = XPCOMUtils.generateNSGetFactory([InformNotikeys]);
else
	const NSGetModule = XPCOMUtils.generateNSGetModule([InformNotikeys]); //for Gecko 1.9.*

var consoleService = Components.classes['@mozilla.org/consoleservice;1'].getService(Components.interfaces.nsIConsoleService);
log("onLoad XPCOMUtils");

var timer = Components.classes["@mozilla.org/timer;1"].createInstance(Components.interfaces.nsITimer);
timer.initWithCallback(event, 5000, Components.interfaces.nsITimer.TYPE_REPEATING_PRECISE_CAN_SKIP);