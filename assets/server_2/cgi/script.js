const form = document.getElementById("cgi_form");
const methodSelect = document.querySelector('select[name="method"]');
const languageSelect = document.querySelector('select[name="language"]');

form.addEventListener("submit", (event) => {
	event.preventDefault(); // Prevent default form submission

	const selectedMethod = methodSelect.value;
	const selectedLanguage = languageSelect.value;

	// Update form attributes
	var extention;
	if (selectedLanguage == "python")
		extention = ".py";
	else if (selectedLanguage == "bash")
		extention = ".sh";
	else if (selectedLanguage == "php")
		extention = ".php";
	form.method = selectedMethod;
	form.action = `/cgi/run-${selectedLanguage}/${selectedMethod}_Default${extention}`;
	form.enctype = "application/x-www-form-urlencoded";


    form.submit();
});
