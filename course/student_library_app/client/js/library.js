let booksData = [];                // Исходный массив всех книг
const uniqueValuesByField = {};    // Словарь множеств уникальных значений по каждому полю

document.addEventListener("DOMContentLoaded", () => {
  loadBooks();

  // Элементы
  const filterTypeSelect = document.getElementById("filter-type");
  const dropdownToggleBtn = document.getElementById("dropdown-toggle-btn");
  const dropdownContent = document.getElementById("dropdown-content");
  const dropdownSearch = document.getElementById("dropdown-search");
  const applyFilterBtn = document.getElementById("apply-filter-btn");
  const sortBtn = document.getElementById("sort-btn");

  // При смене "Filter by" — генерируем чекбоксы
  filterTypeSelect.addEventListener("change", () => {
    // Скрываем/сбрасываем меню при смене типа
    dropdownContent.classList.remove("show");
    onFilterTypeChange();
  });

  // Кнопка раскрытия/скрытия дропдауна
  dropdownToggleBtn.addEventListener("click", () => {
    dropdownContent.classList.toggle("show");
  });

  // Поиск по чекбоксам внутри дропдауна
  dropdownSearch.addEventListener("input", () => {
    filterCheckBoxList(dropdownSearch.value.trim());
  });

  // Применение фильтра
  applyFilterBtn.addEventListener("click", applyFilter);

  // Сортировка
  sortBtn.addEventListener("click", applySort);

  // Если пользователь кликает вне дропдауна, мы закрываем меню
  document.addEventListener("click", (event) => {
    if (!dropdownContent.contains(event.target) && !dropdownToggleBtn.contains(event.target)) {
      dropdownContent.classList.remove("show");
    }
  });
});

/**
 * Загрузка книг с сервера
 */
async function loadBooks() {
  const token = localStorage.getItem("token");
  if (!token) {
    alert("Unauthorized. Please login.");
    window.location.href = "login.html";
    return;
  }

  try {
    const response = await fetch("http://127.0.0.1:8000/books/", {
      headers: { Authorization: `Bearer ${token}` },
    });

    if (!response.ok) {
      throw new Error("Failed to load books");
    }

    booksData = await response.json();
    // Собираем уникальные значения для фильтров
    buildUniqueValues(booksData);

    // Отображаем все книги по умолчанию
    displayBooks(booksData);
  } catch (error) {
    console.error(error);
    alert("Error loading books");
  }
}

/**
 * Собираем уникальные значения (authors, teacher, publisher, city, year, title)
 * Теперь publisher = publisher_rel.name, city = publisher_rel.city
 */
function buildUniqueValues(books) {
  uniqueValuesByField.authors = new Set();
  uniqueValuesByField.teacher = new Set();
  uniqueValuesByField.publisher = new Set();         // name издательства
  uniqueValuesByField.publication_city = new Set();  // city издательства
  uniqueValuesByField.publication_year = new Set();
  uniqueValuesByField.title = new Set();

  books.forEach((book) => {
    // Authors
    if (book.authors && Array.isArray(book.authors)) {
      book.authors.forEach((a) => {
        const fullName = `${a.last_name} ${a.first_name}${a.middle_name ? " " + a.middle_name : ""}`.trim();
        uniqueValuesByField.authors.add(fullName);
      });
    }
    // Teacher
    if (book.owner_username) {
      uniqueValuesByField.teacher.add(book.owner_username);
    }

    // Publisher (берём из book.publisher_rel?.name)
    if (book.publisher_rel && book.publisher_rel.name) {
      uniqueValuesByField.publisher.add(book.publisher_rel.name);
    }

    // City (берём из book.publisher_rel?.city)
    if (book.publisher_rel && book.publisher_rel.city) {
      uniqueValuesByField.publication_city.add(book.publisher_rel.city);
    }

    // Year
    if (book.publication_year) {
      uniqueValuesByField.publication_year.add(String(book.publication_year));
    }

    // Title
    if (book.title) {
      uniqueValuesByField.title.add(book.title);
    }
  });

  // Превращаем множества в массивы (и сортируем)
  for (let field in uniqueValuesByField) {
    uniqueValuesByField[field] = Array.from(uniqueValuesByField[field]).sort();
  }
}

/**
 * Генерация чекбоксов при смене типа фильтра
 */
function onFilterTypeChange() {
  const filterType = document.getElementById("filter-type").value;
  const dropdownCheckboxList = document.getElementById("dropdown-checkbox-list");
  const dropdownSearch = document.getElementById("dropdown-search");

  // Очищаем старый список
  dropdownCheckboxList.innerHTML = "";
  dropdownSearch.value = "";

  if (!filterType) {
    return;
  }

  const uniqueValues = uniqueValuesByField[filterType] || [];

  uniqueValues.forEach((val) => {
    const label = document.createElement("label");
    const cb = document.createElement("input");
    cb.type = "checkbox";
    cb.value = val;
    label.appendChild(cb);
    label.appendChild(document.createTextNode(val));
    dropdownCheckboxList.appendChild(label);
  });
}

/**
 * Поиск по списку чекбоксов (чтобы отфильтровать варианты)
 */
function filterCheckBoxList(searchStr) {
  const dropdownCheckboxList = document.getElementById("dropdown-checkbox-list");
  const labels = dropdownCheckboxList.querySelectorAll("label");

  labels.forEach((lbl) => {
    const text = lbl.textContent.toLowerCase();
    if (text.includes(searchStr.toLowerCase())) {
      lbl.style.display = "";
    } else {
      lbl.style.display = "none";
    }
  });
}

/**
 * Применяем фильтр
 */
function applyFilter() {
  const filterType = document.getElementById("filter-type").value;
  const dropdownCheckboxList = document.getElementById("dropdown-checkbox-list");

  if (!filterType) {
    displayBooks(booksData);
    return;
  }

  // Собираем отмеченные чекбоксы
  const checkedValues = Array.from(
    dropdownCheckboxList.querySelectorAll("input[type='checkbox']:checked")
  ).map(cb => cb.value);

  if (checkedValues.length === 0) {
    // Если ни один не выбран, показываем все
    displayBooks(booksData);
    return;
  }

  // Фильтруем
  let filtered = booksData.filter((book) => {
    switch (filterType) {
      case "authors": {
        if (!book.authors || book.authors.length === 0) return false;
        const authorsFullNames = book.authors.map((a) =>
          `${a.last_name} ${a.first_name}${a.middle_name ? " " + a.middle_name : ""}`.trim()
        );
        return authorsFullNames.some((name) => checkedValues.includes(name));
      }
      case "teacher":
        return checkedValues.includes(book.owner_username);

      case "publisher":
        // book.publisher_rel?.name
        if (!book.publisher_rel) return false;
        return checkedValues.includes(book.publisher_rel.name);

      case "publication_city":
        // book.publisher_rel?.city
        if (!book.publisher_rel) return false;
        return checkedValues.includes(book.publisher_rel.city);

      case "publication_year":
        return checkedValues.includes(String(book.publication_year));

      case "title":
        return checkedValues.includes(book.title);

      default:
        return true;
    }
  });

  displayBooks(filtered);
}

/**
 * Отображение (рендер) книг в списке
 */
function displayBooks(books) {
  const bookList = document.getElementById("book-list");
  if (!bookList) return;
  bookList.innerHTML = "";

  books.forEach((book) => {
    // Издательство (name) и город (city)
    const publisherName = book.publisher_rel ? book.publisher_rel.name : "N/A";
    const publisherCity = book.publisher_rel ? book.publisher_rel.city : "N/A";

    // Авторы
    let authorsText = "N/A";
    if (book.authors && book.authors.length > 0) {
      authorsText = book.authors
        .map(
          (a) =>
            `${a.last_name} ${a.first_name}${a.middle_name ? " " + a.middle_name : ""}`
        )
        .join(", ");
    }

    const li = document.createElement("li");
    li.innerHTML = `
      <strong>Title:</strong> ${book.title || "N/A"} <br>
      <strong>Authors:</strong> ${authorsText} <br>
      <strong>Publisher:</strong> ${publisherName} <br>
      <strong>City:</strong> ${publisherCity} <br>
      <strong>Year:</strong> ${book.publication_year ?? "N/A"} <br>
      <strong>Pages:</strong> ${book.page_count ?? "N/A"} <br>
      <strong>Additional Info:</strong> ${book.additional_info || "N/A"} <br>
      <strong>Download Link:</strong> ${
        book.download_link
          ? `<a href="${book.download_link}" target="_blank">${book.download_link}</a>`
          : "N/A"
      } <br>
      <strong>Teacher:</strong> ${book.owner_username || "N/A"} <br>
    `;
    bookList.appendChild(li);
  });
}

/**
 * Сортировка
 */
function applySort() {
  const sortType = document.getElementById("sort-type").value;
  if (!sortType) {
    displayBooks(booksData);
    return;
  }

  let sorted = [...booksData];

  switch (sortType) {
    case "title":
      sorted.sort((a, b) => (a.title || "").localeCompare(b.title || ""));
      break;
    case "authors":
      // Сортируем по фамилии первого автора
      sorted.sort((a, b) => {
        const aAuthor = (a.authors && a.authors[0])
          ? a.authors[0].last_name.toLowerCase()
          : "";
        const bAuthor = (b.authors && b.authors[0])
          ? b.authors[0].last_name.toLowerCase()
          : "";
        return aAuthor.localeCompare(bAuthor);
      });
      break;
    case "publication_year":
      // По убыванию
      sorted.sort((a, b) => (b.publication_year || 0) - (a.publication_year || 0));
      break;
    case "teacher":
      sorted.sort((a, b) => {
        const aTeacher = (a.owner_username || "").toLowerCase();
        const bTeacher = (b.owner_username || "").toLowerCase();
        return aTeacher.localeCompare(bTeacher);
      });
      break;
    default:
      break;
  }

  displayBooks(sorted);
}

/**
 * Выход (logout)
 */
function logout() {
  localStorage.removeItem("token");
  localStorage.removeItem("role");
  window.location.href = "login.html";
}
