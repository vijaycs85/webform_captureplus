/**
 * Initiates the capture plus object.
 */
(function ($) {
    Drupal.behaviors.webform_captureplus = Drupal.behaviors.webform_captureplus || {};
    Drupal.behaviors.webform_captureplus.attach = function() {
        // create quasi static var to save perfomance
        $.each(Drupal.settings, function(key, info) {
            if(key.substring(0, 20) == 'webform_captureplus_') {
                info.control = new pca.Address(info.fields, info);

            }
        });
        return;
    };

}(jQuery));
