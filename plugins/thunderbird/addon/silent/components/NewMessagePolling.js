Components.utils.import("resource://gre/modules/XPCOMUtils.jsm");
var consoleServicwe = Components.classes['@mozilla.org/consoleservice;1'].getService(Components.interfaces.nsIConsoleService);

function InformNotikeys() {
	consoleServicwe.logStringMessage("onLoad Blocker");
};

InformNotikeys.prototype = {
	classDescription: "Notikeys XPCOM component",
	classID: Components.ID("{11040270-a2e9-11e0-8264-0800200c9a66}"),
	contractID: "@schuzak.jp/SilentBlock;1",
	
	QueryInterface: XPCOMUtils.generateQI(),
};

var event = {
	notify: function(timer) {
		consoleServicwe.logStringMessage("onLoad notify");
		var acctMgr = Components.classes["@mozilla.org/messenger/account-manager;1"].getService(Components.interfaces.nsIMsgAccountManager);
		var accounts = acctMgr.accounts;
		consoleServicwe.logStringMessage("accounts");
		if (accounts.queryElementAt) {
			// Gecko 17+
			consoleServicwe.logStringMessage("Gecko 17+");
			for (var i = 0; i < accounts.length; i++) {
				var account = accounts.queryElementAt(i, Components.interfaces.nsIMsgAccount);
				var rootFolder = account.incomingServer.rootFolder; // nsIMsgFolder
				consoleServicwe.logStringMessage("Root folder: " + rootFolder.prettiestName + ". Unread: " + rootFolder.getNumUnread(true));
				consoleServicwe.logStringMessage("Root folder: " + rootFolder.prettiestName + ". Flags: 0x" + rootFolder.flags.toString(16) + ". Server type: " + rootFolder.server.type);
			}
		}
	}
}

if (XPCOMUtils.generateNSGetFactory)
	const NSGetFactory = XPCOMUtils.generateNSGetFactory([InformNotikeys]);
else
	const NSGetModule = XPCOMUtils.generateNSGetModule([InformNotikeys]); //for Gecko 1.9.*

var consoleService = Components.classes['@mozilla.org/consoleservice;1'].getService(Components.interfaces.nsIConsoleService);
consoleService.logStringMessage("onLoad XPCOMUtils");

var timer = Components.classes["@mozilla.org/timer;1"].createInstance(Components.interfaces.nsITimer);
timer.initWithCallback(event, 3000, Components.interfaces.nsITimer.TYPE_REPEATING_PRECISE_CAN_SKIP);