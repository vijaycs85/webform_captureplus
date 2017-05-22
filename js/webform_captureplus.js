/**
 * Initiates the capture plus object.
 */
(function ($) {
    Drupal.behaviors.webform_captureplus = Drupal.behaviors.webform_captureplus || {};
    Drupal.behaviors.webform_captureplus.attach = function(context, settings) {
        $('body', context).once('webform_captureplus', function () {
            $.each(settings, function (settings_id, settings) {
                if (settings_id.substring(0, 20) == 'webform_captureplus_') {
                    $.each(settings, function (key, info) {
                        info.control = new pca.Address(info.fields, info);
                        attachMessages(info);
                        subscribeDisplayEvent(info);
                    });
                }
            });
        });

        /**
         * Attach translated messages.
         *
         * @param info
         */
        function attachMessages(info) {
            if (info.hasOwnProperty('messages') && info.messages !== null) {
                $.each(info.messages, function(language_id, messages) {
                    $.each(messages, function(message_id, message){
                        pca.messages[language_id][message_id] = message;
                    });
                });
            }
        }

        /**
         * Subscribe to display event.
         *
         *  @param info
         */
        function subscribeDisplayEvent(info) {
            // Display header message, if any.
            if (info.hasOwnProperty('header') && info.header !== null) {
                info.control.listen('display', function (items, attributes) {
                    $.each(info.header, function(header_id, message) {
                        info.control.message(pca.formatLine(attributes, message));
                    });
                });
            }
        }

    };

}(jQuery));
